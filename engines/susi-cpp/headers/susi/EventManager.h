#ifndef __EVENTSMANAGER__
#define __EVENTSMANAGER__

#include <functional>
#include <mutex>
#include <chrono>
#include "susi/ThreadPool.h"
#include "bson/Value.h"
#include "susi/Event.h"
#include "susi/ConstrainedScheduler.h"
#include <regex>

namespace Susi {

//Event pointer types
typedef std::unique_ptr<Event, std::function<void( Event* )>> EventPtr;
typedef std::shared_ptr<Event> SharedEventPtr;

//Callback types
typedef std::function<void( EventPtr )> Processor;
typedef std::function<void( SharedEventPtr )> Consumer;

//a event predicate
typedef std::function<bool( Event& )> Predicate;

class EventManager {
  public:
    EventManager( size_t workers = 4) : pool {workers} {};
    // public subscribe api
    virtual long subscribe( std::string topic, Processor processor, std::string name = "" );
    virtual long subscribe( Predicate pred, Processor processor, std::string name = "" );
    virtual long subscribe( std::string topic, Consumer consumer, std::string name = "" );
    virtual long subscribe( Predicate pred, Consumer consumer, std::string name = "" );

    virtual long registerProcessor( std::string topic, Processor processor, std::string name = "" ) {
        return subscribe(topic, processor, name);
    }
    virtual long registerConsumer( std::string topic, Consumer consumer, std::string name = "" ) {
        return subscribe(topic, consumer, name);
    }
    virtual long registerProcessor( Predicate pred, Processor processor, std::string name = "" ) {
        return subscribe(pred, processor, name);
    }
    virtual long registerConsumer( Predicate pred, Consumer consumer, std::string name = "" ) {
        return subscribe(pred, consumer, name);
    }
    virtual bool unregister( long id );
    // public publish api function
    virtual void publish( EventPtr event, Consumer finishCallback = Consumer {}, bool processorsOnly = false, bool consumersOnly = false );
    // pass event back to system
    virtual void ack( EventPtr event );
    // pass event back to system and dismiss
    virtual void dismiss( EventPtr event );

    void addConstraint( std::pair<std::string, std::string> constraint ) {
        scheduler.addConstraint( constraint );
    }

    virtual EventPtr createEvent( std::string topic );
    virtual EventPtr createEvent( BSON::Value & event );

    void join() {
        pool.join();
    }

  protected:
    Susi::ThreadPool pool;
    std::mutex mutex;
    Susi::ConstrainedScheduler scheduler;

    std::condition_variable publishFinished;

    long subscribe( std::string topic,
                    Predicate predicate,
                    Consumer consumer,
                    Processor processor,
                    std::string name );

    struct PublishProcess {
        std::vector<Processor>   processors;
        std::vector<Consumer>    consumers;
        std::vector<std::string> errors;
        std::mutex               mutex;
        size_t current = 0;
    };
    std::map<std::string, std::shared_ptr<PublishProcess>> publishProcesses;

    struct Subscription {
        //identifier
        long id = 0;
        std::string name = "";
        //select statements
        std::string topic = "";
        Predicate predicate = Predicate {};
        //actual callback
        Processor processor = Processor {};
        Consumer consumer = Consumer {};
    };

    std::map<std::string, std::vector<Subscription>> subscriptionsByTopic;
    std::vector<Subscription> subscriptionsByPred;

    void deleter( Event *event );

};

}//end namespace Susi

#endif // __EVENTSMANAGER__
