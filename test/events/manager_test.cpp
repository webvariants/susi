#include "gtest/gtest.h"
#include "events/global.h"
#include <condition_variable>
#include <chrono>
#include <initializer_list>

class EventManagerTest : public ::testing::Test {
protected:
	std::mutex mutex;
	bool callbackCalledOne = false;
	std::condition_variable condOne;
	bool callbackCalledTwo = false;
	std::condition_variable condTwo;
	virtual void SetUp() override {
		world.setupEventManager();
	}
};

using namespace Susi::Events;

TEST_F(EventManagerTest,SubscribeOneAndFinish){
	subscribe("test",[this](Susi::Events::EventPtr event){
		callbackCalledOne = true;
		condOne.notify_all();
		ack(std::move(event));
	});
	auto event = createEvent("test");
	publish(std::move(event),[this](Susi::Events::EventPtr event){
		callbackCalledTwo = true;
		condTwo.notify_all();
	});
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

TEST_F(EventManagerTest,SubscribeOneAndFinishWithoutAck){
	subscribe("test",[this](Susi::Events::EventPtr event){
		callbackCalledOne = true;
		condOne.notify_all();
	});
	auto event = createEvent("test");
	publish(std::move(event),[this](Susi::Events::EventPtr event){
		callbackCalledTwo = true;
		condTwo.notify_all();
	});
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


TEST_F(EventManagerTest,SubscribeMultipleAndFinish){
	subscribe("test",[this](Susi::Events::EventPtr event){
		event->headers.push_back(std::make_pair("foo","0"));
		ack(std::move(event));
	});
	subscribe("test",[this](Susi::Events::EventPtr event){
		event->headers.push_back(std::make_pair("foo","1"));
		//ack(std::move(event)); // we forgot to ack!
	});
	subscribe("test",[this](Susi::Events::EventPtr event){
		event->headers.push_back(std::make_pair("foo","2"));
		ack(std::move(event));
	});
	auto event = createEvent("test");
	publish(std::move(event),[this](Susi::Events::EventPtr  event){
		EXPECT_EQ("0",event->headers[0].second);
		EXPECT_EQ("1",event->headers[1].second);
		EXPECT_EQ("2",event->headers[2].second);
		callbackCalledOne = true;
		condOne.notify_all();
	});
	{
		std::unique_lock<std::mutex> lk(mutex);
		condOne.wait_for(lk,std::chrono::milliseconds{100},[this](){return callbackCalledOne;});
		EXPECT_TRUE(callbackCalledOne);
	}
}


TEST_F(EventManagerTest,AckInFinishHandler){
	subscribe("topic2",[this](Susi::Events::EventPtr event){
		event->headers.push_back(std::make_pair("foo","bar"));
		ack(std::move(event));
	});

	subscribe("topic1",[this](Susi::Events::EventPtr event){
		auto subEvent = createEvent("topic2");
		struct Callback {
			Callback(Susi::Events::EventPtr evt) : event{std::move(evt)} {}
			Callback(Callback && other) : event{std::move(other.event)} {}
			Callback(Callback & other) : event{std::move(other.event)} {}
			Susi::Events::EventPtr event;
			void operator()(Susi::Events::EventPtr subEvent){
				event->headers.push_back(subEvent->headers[0]);
				ack(std::move(subEvent));
				ack(std::move(event));
			}
		};
		Callback callback(std::move(event));
		publish(std::move(subEvent),callback);
	});

	auto event = createEvent("topic1");
	publish(std::move(event),[this](Susi::Events::EventPtr event){
		EXPECT_TRUE(event->headers.size()>0);
		if(event->headers.size()>0){
			EXPECT_EQ("foo",event->headers[0].first);
			EXPECT_EQ("bar",event->headers[0].second);
		}
		callbackCalledOne = true;
		condOne.notify_all();
	});
	
	{
		std::unique_lock<std::mutex> lk(mutex);
		condOne.wait_for(lk,std::chrono::milliseconds{100},[this](){return callbackCalledOne;});
		EXPECT_TRUE(callbackCalledOne);
	}	
}

