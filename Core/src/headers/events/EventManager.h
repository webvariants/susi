#ifndef __EVENTSMANAGER__
#define __EVENTSMANAGER__

#include <functional>
#include <mutex>
#include <chrono>
#include "util/ThreadPool.h"
#include "util/Any.h"
#include "events/Event.h"
#include "util/Glob.h"

namespace Susi {
namespace Events{

//Event pointer types
typedef std::unique_ptr<Event,std::function<void(Event*)>> EventPtr;
typedef std::shared_ptr<Event> SharedEventPtr;

//Callback types
typedef std::function<void(EventPtr)> Processor;
typedef std::function<void(SharedEventPtr)> Consumer;

//a event predicate
typedef std::function<bool(Event&)> Predicate;

class Manager {
public:
	Manager(size_t workers = 4, size_t buffsize = 32) : pool{workers,buffsize} {};
	// public subscribe api
	long subscribe(std::string topic, Processor processor, char minAuthlevel=0, std::string name="", std::vector<std::string> runBeforeThis = std::vector<std::string>{} , std::vector<std::string> runAfterThis = std::vector<std::string>{});
	long subscribe(Predicate pred, Processor processor, char minAuthlevel=0, std::string name="", std::vector<std::string> runBeforeThis = std::vector<std::string>{} , std::vector<std::string> runAfterThis = std::vector<std::string>{});
	long subscribe(std::string topic, Consumer consumer, char minAuthlevel=0, std::string name="", std::vector<std::string> runBeforeThis = std::vector<std::string>{} , std::vector<std::string> runAfterThis = std::vector<std::string>{});
	long subscribe(Predicate pred, Consumer consumer, char minAuthlevel=0, std::string name="", std::vector<std::string> runBeforeThis = std::vector<std::string>{} , std::vector<std::string> runAfterThis = std::vector<std::string>{});
	bool unsubscribe(long id);
	// public publish api function
	void publish(EventPtr event, Consumer finishCallback = Consumer{});
	// pass event back to system
	void ack(EventPtr event);

	EventPtr createEvent(std::string topic);

protected:
	Susi::Util::ThreadPool pool;
	std::mutex mutex;

	std::condition_variable publishFinished;

	long subscribe(std::string topic, 
				   Predicate predicate,
				   Consumer consumer,
				   Processor processor,
				   char authlevel,
				   std::string name,
				   std::vector<std::string> runBeforeThis,
				   std::vector<std::string> runAfterThis);

	struct PublishProcess {
		std::vector<Processor>   processors;
		std::vector<Consumer>    consumers;
		std::vector<std::string> errors;
		std::mutex				 mutex;
		size_t current = 0;
	};
	std::map<long,std::shared_ptr<PublishProcess>> publishProcesses;

	struct Subscription {
		//identifier
		long id = 0;
		std::string name = "";
		char minAuthlevel = 0;
		//select statements
		std::string topic = "";
		Predicate predicate = Predicate{};
		//actual callback
		Processor processor = Processor{};
		Consumer consumer = Consumer{};
		//constraints
		std::vector<std::string> runBeforeThis;
		std::vector<std::string> runAfterThis;
	};

	std::map<std::string,std::vector<Subscription>> subscriptionsByTopic;
	std::vector<Subscription> subscriptionsByPred;

	std::map<std::string,std::vector<std::pair<long,Processor>>> processorsByTopic;
	std::map<std::string,std::vector<std::pair<long,Consumer>>> consumersByTopic;
	std::vector<std::tuple<long,Predicate,Processor>> processorsByPred;
	std::vector<std::tuple<long,Predicate,Consumer>> consumersByPred;

	void deleter(Event *event);

};

}//end namespace Events
}//end namespace Susi

#endif // __EVENTSMANAGER__
