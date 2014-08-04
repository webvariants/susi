#ifndef __MANAGER__
#define __MANAGER__

#include <functional>
#include <mutex>
#include <chrono>
#include "util/ThreadPool.h"
#include "util/Any.h"

namespace Susi {
namespace Events{

//basic Event definition
typedef std::pair<std::string,std::string> Header;
struct Event {
	long id;
	std::string topic;
	std::vector<Header> headers;
	Susi::Util::Any payload;
};

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
	long subscribe(std::string topic, Processor processor);
	long subscribe(Predicate pred, Processor processor);
	long subscribe(std::string topic, Consumer consumer);
	long subscribe(Predicate pred, Consumer consumer);
	bool unsubscribe(long id);
	// public publish api function
	void publish(EventPtr event, Consumer finishCallback = Consumer{});
	// pass event back to system
	void ack(EventPtr event);

protected:
	Susi::Util::ThreadPool pool;
	std::mutex mutex;

	struct PublishProcess {
		std::vector<Processor>   processors;
		std::vector<Consumer>    consumers;
		std::vector<std::string> errors;
		size_t current = 0;
	};
	std::map<long,std::shared_ptr<PublishProcess>> publishProcesses;

	std::map<std::string,std::vector<std::pair<long,Processor>>> processorsByTopic;
	std::map<std::string,std::vector<std::pair<long,Consumer>>> consumersByTopic;
	std::vector<std::tuple<long,Predicate,Processor>> processorsByPred;
	std::vector<std::tuple<long,Predicate,Consumer>> consumersByPred;
};

}//end namespace Events
}//end namespace Susi

#endif // __MANAGER__