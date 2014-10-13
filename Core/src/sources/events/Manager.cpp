#include "events/EventManager.h"


long Susi::Events::Manager::subscribe(
    std::string topic,
    Predicate predicate,
    Consumer consumer,
    Processor processor,
    char authlevel,
    std::string name )
{
    //Susi::Logger::debug("subscribe something to topic "+topic+" with autlevel "+std::to_string(int(authlevel)));
    long id = std::chrono::system_clock::now().time_since_epoch().count();
    Subscription sub;
    sub.id = id;
    sub.name = name;
    sub.authlevel = authlevel;
    sub.topic = topic;
    sub.predicate = std::move( predicate );
    sub.processor = std::move( processor );
    sub.consumer = std::move( consumer );
    if( topic != "" ) {
        std::lock_guard<std::mutex> lock( mutex );
        auto & subs = subscriptionsByTopic[topic];
        subs.push_back( std::move( sub ) );
    }
    else {
        std::lock_guard<std::mutex> lock( mutex );
        subscriptionsByPred.push_back( std::move( sub ) );
    }
    return id;

}

long Susi::Events::Manager::subscribe( std::string topic, Processor processor, char authlevel, std::string name ) {
    if( Susi::Util::Glob::isGlob( topic ) ) {
        auto predicate = [topic]( Susi::Events::Event& event ) {
            Susi::Util::Glob glob {topic};
            return glob.match( event.topic );
        };
        return subscribe( "",predicate,Consumer {},std::move( processor ),authlevel,name );
    }
    return subscribe( topic,Predicate {},Consumer {},std::move( processor ),authlevel,name );
}

long Susi::Events::Manager::subscribe( Predicate pred, Processor processor, char authlevel, std::string name ) {
    return subscribe( "",pred,Consumer {},std::move( processor ),authlevel,name );
}

long Susi::Events::Manager::subscribe( std::string topic, Consumer consumer, char authlevel, std::string name ) {
    if( Susi::Util::Glob::isGlob( topic ) ) {
        auto predicate = [topic]( Susi::Events::Event& event ) {
            Susi::Util::Glob glob {topic};
            return glob.match( event.topic );
        };
        return subscribe( "",predicate,std::move( consumer ),Processor {},authlevel,name );
    }
    return subscribe( topic,Predicate {},std::move( consumer ),Processor {},authlevel,name );
}

long Susi::Events::Manager::subscribe( Predicate pred, Consumer consumer, char authlevel, std::string name ) {
    return subscribe( "",pred,std::move( consumer ),Processor {},authlevel,name );
}

bool Susi::Events::Manager::unsubscribe( long id ) {
    std::lock_guard<std::mutex> lock( mutex );
    for( auto & kv : subscriptionsByTopic ) {
        auto & subs = kv.second;
        for( int i=0; i<subs.size(); i++ ) {
            if( subs[i].id == id ) {
                subs.erase( subs.begin()+i );
                return true;
            }
        }
    }
    for( int i=0; i<subscriptionsByPred.size(); i++ ) {
        if( subscriptionsByPred[i].id == id ) {
            subscriptionsByPred.erase( subscriptionsByPred.begin()+i );
            return true;
        }
    }
    return false;
}

// public publish api function
void Susi::Events::Manager::publish( Susi::Events::EventPtr event, Susi::Events::Consumer finishCallback ) {
    if( event.get()==nullptr ) {
        //std::cout<<"event is nullptr"<<std::endl;
        event.release();
        return;
    }
    {
        std::lock_guard<std::mutex> lock( mutex );
        auto process = std::make_shared<PublishProcess>();

        std::vector<Subscription> affectedProcessorSubscriptions;
        //collect consumers, processors by topic
        for( auto & kv : subscriptionsByTopic ) {
            if( kv.first == event->topic ) {
                for( auto & sub : kv.second ) {
                    if( event->authlevel >= sub.authlevel ) {
                        if( sub.consumer ) {
                            process->consumers.push_back( sub.consumer );
                        }
                        else if( sub.processor ) {
                            affectedProcessorSubscriptions.push_back( sub );
                        }
                    }else{
                        Susi::Logger::debug("can not use sub for this event: processor authlevel: "+std::to_string(sub.authlevel));
                    }
                }
                break;
            }
        }
        //collect consumers, processors by predicate
        for( auto & sub : subscriptionsByPred ) {
            if( sub.predicate( *event ) ) {
                if( event->authlevel == sub.authlevel ) {
                    if( sub.consumer ) {
                        process->consumers.push_back( sub.consumer );
                    }
                    else if( sub.processor ) {
                        affectedProcessorSubscriptions.push_back( sub );
                    }
                }else{
                    Susi::Logger::debug("can not use sub for this event: processor authlevel: "+std::to_string(sub.authlevel));
                }
            }
        }
        //schedule processors
        std::set<std::string> tasks;
        for( auto & s : affectedProcessorSubscriptions ) {
            tasks.insert( s.name );
        }
        auto plan = scheduler.schedule( tasks );
        for( auto & name : plan ) {
            for( auto & sub : affectedProcessorSubscriptions ) {
                if( sub.name==name ) {
                    process->processors.push_back( sub.processor );
                }
            }
        }

        process->consumers.push_back( finishCallback );
        publishProcesses[event->id] = process;
    } // release lock
    ack( std::move( event ) );
}

// pass event back to system
void Susi::Events::Manager::ack( EventPtr event ) {
    if( event.get()==nullptr ) {
        event.release();
        return;
    }
    struct Work {
        EventPtr event;
        Manager *manager;
        Work( EventPtr evt, Manager *mgr ) : event {std::move( evt )}, manager {mgr} {}
        Work( Work && other ) : event {std::move( other.event )}, manager {other.manager} {}
        Work( Work & other ) : event {std::move( other.event )}, manager {other.manager} {}
        void operator()() {
            long id = event->id;
            std::shared_ptr<PublishProcess> process;
            {
                std::unique_lock<std::mutex> lock( manager->mutex );
                for( auto & kv : manager->publishProcesses ) {
                    if( kv.first == id ) {
                        process = kv.second;
                        break;
                    }
                }
            }
            if( process.get()==nullptr ) {
                /*std::cout<<"cant find process, this should not happen"<<std::endl;
                std::cout<<event->topic<<std::endl;*/
                delete event.release();
                return;
            }
            //std::cout<<"ack event "<<event->topic<<std::endl;

            std::unique_lock<std::mutex> lock( process->mutex );
            while( process->errors.size() > 0 ) {
                event->headers.push_back( std::make_pair( "error",process->errors.back() ) );
                process->errors.pop_back();
            }
            if( process->current < process->processors.size() ) {
                try {
                    auto nextProcessor = process->processors[process->current++];
                    if( nextProcessor ) {
                        nextProcessor( std::move( event ) );
                    }
                }
                catch( const std::exception & e ) {
                    process->errors.push_back( e.what() );
                }
            }
            else {
                std::shared_ptr<Event> sharedEvent {event.release()};
                for( auto & consumer : process->consumers ) {
                    if( consumer ) manager->pool.add( [consumer,sharedEvent]() {
                        consumer( sharedEvent );
                    } /*,[this](std::string errorMessage){
                        auto errorEvent = manager->createEvent("consumererror");
                        Susi::Logger::error("consumer error: "+errorMessage);
                        errorEvent->payload = errorMessage;
                        manager->publish(std::move(errorEvent));
                    }*/);
                    //@TODO: the error handler results in a segfault. inspect why and solve it!
                }
                {
                    std::unique_lock<std::mutex> lock( manager->mutex );
                    for( auto & kv : manager->publishProcesses ) {
                        if( kv.second == process ) {
                            manager->publishProcesses.erase( kv.first );
                            break;
                        }
                    }
                }
                manager->publishFinished.notify_one();
            }
        }
    };

    long id = event->id;
    auto error = [id,this]( std::string msg ) {
        std::unique_lock<std::mutex> lock( mutex );
        std::shared_ptr<PublishProcess> process;
        for( auto & kv : publishProcesses ) {
            if( kv.first == id ) {
                process = kv.second;
            }
        }
        if( process.get()!=nullptr ) {
            process->errors.push_back( msg );
        }
    };
    Work work {std::move( event ),this};
    pool.add( std::move( work ),error );
}

Susi::Events::EventPtr Susi::Events::Manager::createEvent( std::string topic ) {
    auto event = Susi::Events::EventPtr {new Susi::Events::Event{topic},[this]( Event * event ) {
            this->deleter( event );
        }
    };
    return event;
}

void Susi::Events::Manager::deleter( Event *event ) {
    //std::cout<<"calling deleter of "<<event<<std::endl;
    if( event!=nullptr ) {
        Susi::Events::EventPtr ptr( event,[this]( Event *event ) {
            deleter( event );
        } );
        try {
            ack( std::move( ptr ) );
        }
        catch( const std::exception & e ) {
            std::cout<<"error in deleter:"<<e.what()<<std::endl;
        }
    }
}
