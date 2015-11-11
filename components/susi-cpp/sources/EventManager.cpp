#include "susi/EventManager.h"


long Susi::EventManager::subscribe(
    std::string topic,
    Predicate predicate,
    Consumer consumer,
    Processor processor,
    std::string name ) {
    long id = std::chrono::system_clock::now().time_since_epoch().count();
    Subscription sub;
    sub.id = id;
    sub.name = name;
    sub.topic = topic;
    sub.predicate = std::move( predicate );
    sub.processor = std::move( processor );
    sub.consumer = std::move( consumer );
    if ( topic != "" ) {
        std::lock_guard<std::mutex> lock( mutex );
        auto & subs = subscriptionsByTopic[topic];
        subs.push_back( std::move( sub ) );
    } else {
        std::lock_guard<std::mutex> lock( mutex );
        subscriptionsByPred.push_back( std::move( sub ) );
    }
    return id;

}

long Susi::EventManager::subscribe( std::string topic, Processor processor, std::string name ) {
    return subscribe( topic, Predicate {}, Consumer {}, std::move( processor ), name );
}

long Susi::EventManager::subscribe( Predicate pred, Processor processor, std::string name ) {
    return subscribe( "", pred, Consumer {}, std::move( processor ), name );
}

long Susi::EventManager::subscribe( std::string topic, Consumer consumer, std::string name ) {
    return subscribe( topic, Predicate {}, std::move( consumer ), Processor {}, name );
}

long Susi::EventManager::subscribe( Predicate pred, Consumer consumer, std::string name ) {
    return subscribe( "", pred, std::move( consumer ), Processor {}, name );
}

bool Susi::EventManager::unregister( long id ) {
    std::lock_guard<std::mutex> lock( mutex );
    for ( auto & kv : subscriptionsByTopic ) {
        auto & subs = kv.second;
        for ( size_t i = 0; i < subs.size(); i++ ) {
            if ( subs[i].id == id ) {
                subs.erase( subs.begin() + i );
                return true;
            }
        }
    }
    for ( size_t i = 0; i < subscriptionsByPred.size(); i++ ) {
        if ( subscriptionsByPred[i].id == id ) {
            subscriptionsByPred.erase( subscriptionsByPred.begin() + i );
            return true;
        }
    }
    return false;
}

// public publish api function
void Susi::EventManager::publish( Susi::EventPtr event, Susi::Consumer finishCallback, bool processorsOnly, bool consumersOnly) {
    if ( event.get() == nullptr ) {
        //std::cout<<"event is nullptr"<<std::endl;
        event.release();
        return;
    }

    //LOG(DEBUG) << "publish!";
    {
        std::lock_guard<std::mutex> lock( mutex );
        auto process = std::make_shared<PublishProcess>();

        std::vector<Subscription> affectedProcessorSubscriptions;
        //collect consumers, processors by topic
        for ( auto & kv : subscriptionsByTopic ) {
            if ( std::regex_match(event->topic, std::regex{kv.first} )) {
                for ( auto & sub : kv.second ) {
                    if ( sub.consumer && !processorsOnly ) {
                        process->consumers.push_back( sub.consumer );
                    } else if ( sub.processor && !consumersOnly ) {
                        affectedProcessorSubscriptions.push_back( sub );
                    }
                }
            }
        }
        //collect consumers, processors by predicate
        for ( auto & sub : subscriptionsByPred ) {
            if ( sub.predicate( *event ) ) {
                if ( sub.consumer ) {
                    process->consumers.push_back( sub.consumer );
                } else if ( sub.processor && !consumersOnly ) {
                    affectedProcessorSubscriptions.push_back( sub );
                }
            }
        }
        //schedule processors
        std::set<std::string> tasks;
        for ( auto & s : affectedProcessorSubscriptions ) {
            tasks.insert( s.name );
        }
        auto plan = scheduler.schedule( tasks );
        for ( auto & name : plan ) {
            for ( auto & sub : affectedProcessorSubscriptions ) {
                if ( sub.name == name ) {
                    process->processors.push_back( sub.processor );
                }
            }
        }

        process->consumers.push_back( finishCallback );
        publishProcesses[event->id] = process;
    } // release lock
    ack( std::move( event ) );
}

void Susi::EventManager::dismiss( EventPtr event ) {
    if ( event.get() == nullptr ) {
        event.release();
        return;
    }
    std::shared_ptr<PublishProcess> process;
    {
        std::unique_lock<std::mutex> lock( mutex );
        for ( auto & kv : publishProcesses ) {
            if ( kv.first == event->id ) {
                process = kv.second;
                break;
            }
        }
    }
    if ( process.get() == nullptr ) {
        /*std::cout<<"cant find process, this should not happen"<<std::endl;
        std::cout<<event->topic<<std::endl;*/
        delete event.release();
        return;
    }
    event->headers.push_back({"Error", "Dismissed"});
    process->current = process->processors.size();
    ack(std::move(event));
}

// pass event back to system
void Susi::EventManager::ack( EventPtr event ) {
    if ( event.get() == nullptr ) {
        event.release();
        return;
    }
    //LOG(DEBUG) << "ack!";
    struct Work {
        EventPtr event;
        EventManager *manager;
        Work( EventPtr evt, EventManager *mgr ) : event {std::move( evt )}, manager {mgr} {}
        Work( Work && other ) : event {std::move( other.event )}, manager {other.manager} {}
        Work( Work & other ) : event {std::move( other.event )}, manager {other.manager} {}
        void operator()() {
            std::string id = event->id;
            std::shared_ptr<PublishProcess> process;
            {
                std::unique_lock<std::mutex> lock( manager->mutex );
                for ( auto & kv : manager->publishProcesses ) {
                    if ( kv.first == id ) {
                        process = kv.second;
                        break;
                    }
                }
            }
            if ( process.get() == nullptr ) {
                /*std::cout<<"cant find process, this should not happen"<<std::endl;
                std::cout<<event->topic<<std::endl;*/
                delete event.release();
                return;
            }
            //std::cout<<"ack event "<<event->topic<<std::endl;

            std::unique_lock<std::mutex> lock( process->mutex );
            while ( process->errors.size() > 0 ) {
                event->headers.push_back( std::make_pair( "Error", process->errors.back() ) );
                process->errors.pop_back();
            }
            if ( process->current < process->processors.size() ) {
                try {
                    auto nextProcessor = process->processors[process->current++];
                    if ( nextProcessor ) {
                        nextProcessor( std::move( event ) );
                    }
                } catch ( const std::exception & e ) {
                    process->errors.push_back( e.what() );
                }
            } else {
                std::shared_ptr<Event> sharedEvent {event.release()};
                for ( auto & consumer : process->consumers ) {
                    if ( consumer ) manager->pool.add( [consumer, sharedEvent]() {
                        consumer( sharedEvent );
                    } /*,[this](std::string errorMessage){
                        auto errorEvent = manager->createEvent("consumererror");
                        LOG(ERROR) << "consumer error: "+errorMessage;
                        errorEvent->payload = errorMessage;
                        manager->publish(std::move(errorEvent));
                    }*/);
                    //@TODO: the error handler results in a segfault. inspect why and solve it!
                }
                {
                    std::unique_lock<std::mutex> lock( manager->mutex );
                    for ( auto & kv : manager->publishProcesses ) {
                        if ( kv.second == process ) {
                            manager->publishProcesses.erase( kv.first );
                            break;
                        }
                    }
                }
                manager->publishFinished.notify_one();
            }
        }
    };

    std::string id = event->id;
    auto error = [id, this]( std::string msg ) {
        try {
            std::unique_lock<std::mutex> lock( mutex );
            std::shared_ptr<PublishProcess> process;
            for ( auto & kv : publishProcesses ) {
                if ( kv.first == id ) {
                    process = kv.second;
                }
            }
            if ( process.get() != nullptr ) {
                process->errors.push_back( msg );
            }
        } catch (...) {
            std::cerr << "Error in error callback of eventmanager work" << std::endl;
        }
    };

    Work work {std::move( event ), this};
    pool.add( std::move( work ), error );
}

Susi::EventPtr Susi::EventManager::createEvent( BSON::Value & evt ) {
    auto event = Susi::EventPtr {new Susi::Event{evt}, [this]( Event * event ) {
            this->deleter( event );
        }
    };
    return event;
}

Susi::EventPtr Susi::EventManager::createEvent( std::string topic ) {
    auto event = Susi::EventPtr {new Susi::Event{topic}, [this]( Event * event ) {
            this->deleter( event );
        }
    };
    return event;
}

void Susi::EventManager::deleter( Event *event ) {
    //std::cout<<"calling deleter of "<<event<<std::endl;
    if ( event != nullptr ) {
        Susi::EventPtr ptr( event, [this]( Event * event ) {
            deleter( event );
        } );
        try {
            ack( std::move( ptr ) );
        } catch ( const std::exception & e ) {
            std::cout << "error in deleter:" << e.what() << std::endl;
        }
    }
}
