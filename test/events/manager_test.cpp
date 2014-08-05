#include "gtest/gtest.h"
#include "events/global.h"
#include "world/World.h"
#include <condition_variable>
#include <chrono>

// This is the EventManagerTest, here you can see how to use the new eventsystem.
class EventManagerTest : public ::testing::Test {
protected:
	// helpers to check if callbacks are fired
	std::mutex mutex;
	bool callbackCalledOne = false;
	std::condition_variable condOne;
	bool callbackCalledTwo = false;
	std::condition_variable condTwo;
	
	// allways start with a fresh eventmanager
	virtual void SetUp() override {
		world.setupEventManager();
	}
};

using namespace Susi::Events;

// You use the event system by two main functions: subscribe(topic,handler) and publish(topic,finishHandler)
// When subscribing to a topic, you attach an event handler to a specific topic / identifier.
// When publishing you pass an event to the eventsystem which will invoke all subscribed handlers.
// These handlers can manipulate the event, and once the system is finished, the finishHandler is called with the 
// manipulated / extended event.
// So out first simple usecase: 
// First subscribe, then publish and wait for the finishCallback to be called.
TEST_F(EventManagerTest,SubscribeOneAndFinish){
	// First parameter is the topic, second is a std::function<void(Susi::Events::EventPtr)>
	// Subscribing a lambda is sufficent for this simple usecase.
	// Since std::function is really good in converting functions of all kinds, we can use a lambda
	subscribe("test",[this](EventPtr event){
		// set the finished condition to true, and notify the main test.
		callbackCalledOne = true;
		condOne.notify_all();
		// acknoledge the event, to say that we are done with it and pass it back to the event system
		ack(std::move(event));
	});

	// create a new EventPtr
	auto event = createEvent("test");
	
	// and publish it! The first parameter is the event you want to publish. 
	// You have to move the EventPtr, because it is an std::unique_ptr.
	// e.g. we only move events, but do not copy them.
	// Second parameter is the finishCallback, which will be fired once the eventsystem is finished with the event
	publish(std::move(event),[this](SharedEventPtr event){
		// set the finished condition to true, and notify the main test.
		callbackCalledTwo = true;
		condTwo.notify_all();
	});

	// ok, if everything works like expected, both installed callbacks
	// (the subscription handler and the finish handler) should be called.
	// therefore we wait on the condition variables.
	{
		// wait for handler callback
		std::unique_lock<std::mutex> lk(mutex);
		condOne.wait_for(lk,std::chrono::milliseconds{100},[this](){return callbackCalledOne;});
		EXPECT_TRUE(callbackCalledOne);
	}
	{
		// wait for finish callback
		std::unique_lock<std::mutex> lk(mutex);
		condTwo.wait_for(lk,std::chrono::milliseconds{100},[this](){return callbackCalledTwo;});
		EXPECT_TRUE(callbackCalledTwo);
	}
}

// It is likely to forget acknoledging the event.
// This could happen, if for example an uncatched exception is thrown.
// For this usecase EventPtr's are able to rescue themself, 
// e.g. they call ack(this) on destruction.
// Therefore it is not fatal but unwelcome to forget to acknowledge an event.
TEST_F(EventManagerTest,SubscribeOneAndFinishWithoutAck){
	// subscribe handler
	subscribe("test",[this](EventPtr event){
		callbackCalledOne = true;
		condOne.notify_all();
		// Here we forget to acknowledge!
	});

	// publish event
	auto event = createEvent("test");
	publish(std::move(event),[this](SharedEventPtr event){
		callbackCalledTwo = true;
		condTwo.notify_all();
	});

	// because of auto acknowledging, both callbacks should also be called.
	{
		std::unique_lock<std::mutex> lk(mutex);
		condOne.wait_for(lk,std::chrono::milliseconds{100},[this](){return callbackCalledOne;});
		EXPECT_TRUE(callbackCalledOne);
	}
	{
		std::unique_lock<std::mutex> lk(mutex);
		condTwo.wait_for(lk,std::chrono::milliseconds{100},[this](){return callbackCalledTwo;});
		EXPECT_TRUE(callbackCalledTwo);
	}
}

// Subscribe multiple handlers to one event.
// You can do so, and the order of subscriptions is the order of event processing.
// Additionally you can see the use of headers in this example/testcase.
// Headers are a vector<pair<string,string>> field in every event. 
// It is designed to carry small peaces of information (like error messages).
TEST_F(EventManagerTest,SubscribeMultipleAndFinish){
	// Install three handlers
	subscribe("test",[this](EventPtr event){
		// add a foo header
		event->headers.push_back(std::make_pair("foo","0"));
		ack(std::move(event));
	});
	subscribe("test",[this](EventPtr event){
		event->headers.push_back(std::make_pair("foo","1"));
		ack(std::move(event)); 
	});
	subscribe("test",[this](EventPtr event){
		event->headers.push_back(std::make_pair("foo","2"));
		ack(std::move(event));
	});

	// publish and check if event manipulation worked like expected
	auto event = createEvent("test");
	publish(std::move(event),[this](SharedEventPtr  event){
		// the event should contain three foo headers with values 1, 2, 3.
		EXPECT_EQ(3,event->headers.size());
		EXPECT_EQ("foo",event->headers[0].first);
		EXPECT_EQ("foo",event->headers[1].first);
		EXPECT_EQ("foo",event->headers[2].first);
		EXPECT_EQ("0",event->headers[0].second);
		EXPECT_EQ("1",event->headers[1].second);
		EXPECT_EQ("2",event->headers[2].second);
		callbackCalledOne = true;
		condOne.notify_all();
	});

	// wait for callback.
	{
		std::unique_lock<std::mutex> lk(mutex);
		condOne.wait_for(lk,std::chrono::milliseconds{100},[this](){return callbackCalledOne;});
		EXPECT_TRUE(callbackCalledOne);
	}
}

// It is possible to acknowledge an event later, for example in the callback of another publish
// Think about a situation where you need the awnser of a subcontroller to awnser a specific event.
// Because EventPtr's are not copyable (remember they are std::unique_ptr) they must be moved into the callback
// Unfortunatly moving of a object into a lambda is not possible with c++11, this is a c++1y feature.
// Therefore we need to use another callable object, in which we can move our event that we want to ack later.
// We can use a simple struct{void operator()(EventPtr){...}} for that.
TEST_F(EventManagerTest,AckInFinishHandler){
	// setup sub controller: add simple header to event.
	subscribe("topic2",[this](EventPtr event){
		event->headers.push_back(std::make_pair("foo","bar"));
		ack(std::move(event));
	});

	// setup main controller: publish sub event to sub controller 
	// and acknowledge main event in finishCallback 
	subscribe("topic1",[this](EventPtr event){
		struct Callback {
			// our main event which we want to acknowledge later
			EventPtr mainEvent;
			// we need this contrusctors to convert this noncopyable to Susi::Events::Handler!
			Callback(EventPtr evt) : mainEvent{std::move(evt)} {}
			Callback(Callback && other) : mainEvent{std::move(other.mainEvent)} {}
			Callback(Callback & other) : mainEvent{std::move(other.mainEvent)} {}
			// the callback corpus
			void operator()(SharedEventPtr subEvent){
				// copy first header of subEvent to mainEvent and acknowledge both.
				mainEvent->headers.push_back(subEvent->headers[0]);
				ack(std::move(mainEvent));
			}
		};
		// instanciate sub event and callback and publish it
		auto subEvent = createEvent("topic2");
		Callback callback(std::move(event));
		publish(std::move(subEvent),callback);
	});

	// publish event to main controller
	auto event = createEvent("topic1");
	publish(std::move(event),[this](SharedEventPtr event){
		// the event must contain a header {foo:bar} which was created by the subcontroller
		// and forwarded by the main controller
		EXPECT_TRUE(event->headers.size()>0);
		if(event->headers.size()>0){
			EXPECT_EQ("foo",event->headers[0].first);
			EXPECT_EQ("bar",event->headers[0].second);
		}
		callbackCalledOne = true;
		condOne.notify_all();
	});
	
	// wait for callback
	{
		std::unique_lock<std::mutex> lk(mutex);
		condOne.wait_for(lk,std::chrono::milliseconds{100},[this](){return callbackCalledOne;});
		EXPECT_TRUE(callbackCalledOne);
	}	
}

// We can unsubscribe a handler.
// For this subscribe() returns an id, which we can use to unsubscribe the installed handler.
TEST_F(EventManagerTest,unsubscribe){
	// subscribe and save id;
	long id = subscribe("test",[this](EventPtr event){
		callbackCalledOne = true;
		condOne.notify_all();
		ack(std::move(event));
	});
	// unsubscribe the handler corresponding to this id
	unsubscribe(id);

	// publish event
	auto event = createEvent("test");
	publish(std::move(event),[this](SharedEventPtr event){
		callbackCalledTwo = true;
		condTwo.notify_all();
	});

	// the first callback should NOT be triggered, because the handler was unsubscribed before publishing
	{
		std::unique_lock<std::mutex> lk(mutex);
		condOne.wait_for(lk,std::chrono::milliseconds{100},[this](){return callbackCalledOne;});
		EXPECT_FALSE(callbackCalledOne);
	}
	// the second (finish)callback should be triggered anyway. 
	{
		std::unique_lock<std::mutex> lk(mutex);
		condTwo.wait_for(lk,std::chrono::milliseconds{100},[this](){return callbackCalledTwo;});
		EXPECT_TRUE(callbackCalledTwo);
	}
}

// We can subscribe to a predicate
// e.g. we can provide a function<bool(const Event &)> to check if we are interested in the event.
TEST_F(EventManagerTest, PredicateSubscribe){
	// setup predicate: here we use a simple equality test (same effect as subscribe("test",handler))
	auto predicate = [](const Event & event){
		return event.topic == "test";
	};
	// setup handler.
	auto handler = [this](EventPtr event){
		callbackCalledOne = true;
		condOne.notify_all();
		ack(std::move(event));
	};
	// subscribe
	subscribe(predicate,handler);

	// publish event
	auto event = createEvent("test");
	publish(std::move(event),[this](SharedEventPtr event){
		callbackCalledTwo = true;
		condTwo.notify_all();
	});

	// wait for handler callback
	{
		std::unique_lock<std::mutex> lk(mutex);
		condOne.wait_for(lk,std::chrono::milliseconds{100},[this](){return callbackCalledOne;});
		EXPECT_TRUE(callbackCalledOne);
	}
	// wait for finish callback 
	{
		std::unique_lock<std::mutex> lk(mutex);
		condTwo.wait_for(lk,std::chrono::milliseconds{100},[this](){return callbackCalledTwo;});
		EXPECT_TRUE(callbackCalledTwo);
	}
}

// We can also unsubscribe a handler which was setup with a predicate
TEST_F(EventManagerTest, PredicateUnsubscribe){
	// setup predicate: here we use a simple equality test (same effect as subscribe("test",handler))
	auto predicate = [](const Event & event){
		return event.topic == "test";
	};
	// setup handler.
	auto handler = [this](EventPtr event){
		callbackCalledOne = true;
		condOne.notify_all();
		ack(std::move(event));
	};
	// subscribe
	long id = subscribe(predicate,handler);
	unsubscribe(id);

	// publish event
	auto event = createEvent("test");
	publish(std::move(event),[this](SharedEventPtr event){
		callbackCalledTwo = true;
		condTwo.notify_all();
	});

	// wait for handler callback (should NOT be called) 
	{
		std::unique_lock<std::mutex> lk(mutex);
		condOne.wait_for(lk,std::chrono::milliseconds{100},[this](){return callbackCalledOne;});
		EXPECT_FALSE(callbackCalledOne);
	}
	// wait for finish callback 
	{
		std::unique_lock<std::mutex> lk(mutex);
		condTwo.wait_for(lk,std::chrono::milliseconds{100},[this](){return callbackCalledTwo;});
		EXPECT_TRUE(callbackCalledTwo);
	}
}

// Lets stress the system. We setup 1000 handlers, and look what's happening.
TEST_F(EventManagerTest,StressTest){
	// setup 1000 handlers
	size_t numOfHandlers = 1000;
	struct Handler {
		size_t num;
		void operator()(EventPtr event){
			event->headers.push_back(std::make_pair("foo",std::to_string(num)));
		}
	};
	for(size_t i=0; i<numOfHandlers; i++){
		subscribe("test",Handler{i});
	}

	//publish
	auto event = createEvent("test");
	publish(std::move(event),[this,numOfHandlers](SharedEventPtr event){
		EXPECT_EQ(numOfHandlers,event->headers.size());
		for(size_t i=0;i<numOfHandlers;i++){
			EXPECT_EQ("foo",event->headers[i].first);
			EXPECT_EQ(std::to_string(i),event->headers[i].second);
		}
		callbackCalledOne = true;
		condOne.notify_all();
	});

	// wait for finish callback 
	{
		std::unique_lock<std::mutex> lk(mutex);
		condOne.wait_for(lk,std::chrono::milliseconds{100},[this](){return callbackCalledOne;});
		EXPECT_TRUE(callbackCalledOne);
	}
}

