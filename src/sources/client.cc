#include <iostream>
#include <mutex>
#include <condition_variable>

// Basically we only need one class to connect to susi...
#include "apiserver/ApiClient.h"

// This is the Susi server address
#define SUSIADDR "[::1]:4000"

int main(){

	using namespace Susi::Api; // for ApiClient
	using namespace Susi::Events;// for EventPtr, SharedEventPtr, Processor, Consumer
	
	// Construct the client with IP address of the susi server.
	ApiClient client{SUSIADDR};
	
	// Create a event processor which handles the "getAlarms"-Event
	Processor alarmsHandler{[&client](EventPtr event){
		// We need to perform a sub-event to talk to the Database Manager
		// -> We need to specify the db (by identifier) and the query.
		auto dbEvent = client.createEvent("db::query");
		dbEvent->payload["identifier"] = "alarms";
		dbEvent->payload["query"] = "select * from alarms;";
		
		// When the db query is finished this Callable will be called...
		// We need to use a struct with operator(), because we need to carry the original request event.
		struct DBQueryCallback {
			EventPtr event;
			ApiClient * client;
			// We need this constructors to be convertable to a Consumer (std::function<void(SharedEventPtr)>)
			DBQueryCallback(EventPtr evt, ApiClient * cli) : event{std::move(evt)}, client{cli} {}
			DBQueryCallback(DBQueryCallback & other) : event{std::move(other.event)}, client{other.client} {}
			DBQueryCallback(DBQueryCallback && other) : event{std::move(other.event)}, client{other.client} {}
			// This is the actual code which is executed when the db query finished.
			void operator()(SharedEventPtr dbAwnserEvent){
				// Here we simply copy the result, and acknowledge (pass back to susi) the original request.
				event->payload = dbAwnserEvent->payload["result"];
				client->ack(std::move(event));
			}
		};

		// Instanciate the finish handler and move request event into it.
		Consumer finishCallback = DBQueryCallback{std::move(event),&client};
		// and publish the dbEvent!
		client.publish(std::move(dbEvent),finishCallback);
	}};
	
	// register our alarmsHandler to the topic "getAlarms".
	// if such an event is triggered, the handler will be invoked
	client.subscribe("getAlarms",alarmsHandler);
	
	// Wait forever (or do something else), because the client runs in a separate thread.
	std::mutex mutex;
	std::condition_variable cond;
	std::unique_lock<std::mutex> lock{mutex};
	cond.wait(lock);
}
