#ifndef __EVENTSMANAGER__
#define __EVENTSMANAGER__

#include <functional>
#include <mutex>
#include <chrono>
#include "util/ThreadPool.h"
#include "util/Any.h"
#include "events/Event.h"
#include "util/Glob.h"
#include "util/ConstrainedScheduler.h"

#include "logger/easylogging++.h"

#include "events/IEventSystem.h"

namespace Susi {
    namespace Events {

        class Manager : public IEventSystem {
        public:
            Manager( size_t workers = 4, size_t buffsize = 32 ) : pool {workers,buffsize} {};
            // public subscribe api
            long subscribe( std::string topic, Processor processor, std::string name="" );
            long subscribe( Predicate pred, Processor processor, std::string name="" );
            long subscribe( std::string topic, Consumer consumer, std::string name="" );
            long subscribe( Predicate pred, Consumer consumer, std::string name="" );
            bool unsubscribe( long id );
            // public publish api function
            void publish( EventPtr event, Consumer finishCallback = Consumer {} );
            // pass event back to system
            void ack( EventPtr event );

            void addConstraint( std::pair<std::string,std::string> constraint ) {
                scheduler.addConstraint( constraint );
            }

            EventPtr createEvent( std::string topic );

        protected:
            Susi::Util::ThreadPool pool;
            std::mutex mutex;
            Susi::Util::ConstrainedScheduler scheduler;

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
            std::map<std::string,std::shared_ptr<PublishProcess>> publishProcesses;

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

            std::map<std::string,std::vector<Subscription>> subscriptionsByTopic;
            std::vector<Subscription> subscriptionsByPred;

            void deleter( Event *event );

        };

    }//end namespace Events
}//end namespace Susi

#endif // __EVENTSMANAGER__
