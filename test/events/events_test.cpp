#include "gtest/gtest.h"
#include "events/EventSystem.h"
#include <condition_variable>
#include "world/World.h"

class EventSystemTest : public ::testing::Test {
protected:
	virtual void SetUp() override {
		world.setupEventSystem();
	}
};

TEST_F(EventSystemTest, Event) {
	auto event = Susi::Event("topic",Poco::Dynamic::Var(5));
	EXPECT_EQ("topic",event.topic);
	EXPECT_TRUE(event.payload.isInteger());
	EXPECT_EQ("",event.sessionID);
	EXPECT_EQ("",event.returnAddr);
}

TEST_F(EventSystemTest, SimplePubSub) {
	bool callbackCalled = false;
	std::condition_variable cond;
	std::mutex m;
	long id = Susi::subscribe("foo",[&cond,&callbackCalled](Susi::Event event){
		callbackCalled = true;
		cond.notify_one();
	});
	Susi::Event event("foo");
	Susi::publish(event);
	{
		std::unique_lock<std::mutex> lk(m);
		cond.wait(lk,[&callbackCalled](){return callbackCalled;});
		EXPECT_TRUE(callbackCalled);
	}

	callbackCalled = false;
	event.topic = "bar";
	Susi::publish(event);
	{
		std::unique_lock<std::mutex> lk(m);
		cond.wait_for(lk,
			std::chrono::duration<int,std::milli>{100},
			[&callbackCalled](){return callbackCalled;});
		EXPECT_FALSE(callbackCalled);
	}

	Susi::unsubscribe("foo",id);
}

TEST_F(EventSystemTest, unsubscribe) {
	bool callbackCalled = false;
	std::condition_variable cond;
	std::mutex m;
	long id = Susi::subscribe("foobar",[&cond,&callbackCalled](Susi::Event event){
		callbackCalled = true;
		cond.notify_one();
	});
	Susi::Event event("foobar");
	Susi::publish(event);
	{
		std::unique_lock<std::mutex> lk(m);
		cond.wait(lk,[&callbackCalled](){return callbackCalled;});
		EXPECT_TRUE(callbackCalled);
	}

	Susi::unsubscribe("foobar",id);

	callbackCalled = false;
	Susi::publish(event);
	{
		std::unique_lock<std::mutex> lk(m);
		cond.wait_for(lk,
			std::chrono::duration<int,std::milli>{100},
			[&callbackCalled](){return callbackCalled;});
		EXPECT_FALSE(callbackCalled);
	}
}

TEST_F(EventSystemTest, SimpleGlobPubSub) {
	bool callbackCalled = false;
	std::condition_variable cond;
	std::mutex m;
	long id = Susi::subscribe("foo*",[&cond,&callbackCalled](Susi::Event event){
		callbackCalled = true;
		cond.notify_one();
	});
	Susi::Event event("foobar");
	Susi::publish(event);
	{
		std::unique_lock<std::mutex> lk(m);
		cond.wait(lk,[&callbackCalled](){return callbackCalled;});
		EXPECT_TRUE(callbackCalled);
	}

	callbackCalled = false;
	event.topic = "fuubar";
	Susi::publish(event);
	{
		std::unique_lock<std::mutex> lk(m);
		cond.wait_for(lk,
			std::chrono::duration<int,std::milli>{100},
			[&callbackCalled](){return callbackCalled;});
		EXPECT_FALSE(callbackCalled);
	}

	Susi::unsubscribe("foo*",id);
}

TEST_F(EventSystemTest, once) {
	bool callbackCalled = false;
	std::condition_variable cond;
	std::mutex m;
	Susi::once("foobar",[&cond,&callbackCalled](Susi::Event event){
		callbackCalled = true;
		cond.notify_all();
	});
	Susi::Event event("foobar");
	Susi::publish(event);
	{
		std::unique_lock<std::mutex> lk(m);
		cond.wait(lk,[&callbackCalled](){return callbackCalled;});
		EXPECT_TRUE(callbackCalled);
	}

	callbackCalled = false;
	Susi::publish(event);
	{
		std::unique_lock<std::mutex> lk(m);
		cond.wait_for(lk,
			std::chrono::duration<int,std::milli>{100},
			[&callbackCalled](){return callbackCalled;});
		EXPECT_FALSE(callbackCalled);
	}
}

TEST_F(EventSystemTest,Request){
	Susi::once("foobar",[](Susi::Event & event){
		Susi::answerEvent(event,Poco::Dynamic::Var(23));
	});
	auto res = Susi::request("foobar");
	EXPECT_TRUE(res.isInteger());
	EXPECT_EQ(23,res.convert<int>());
	res = Susi::request("foobar");
	EXPECT_TRUE(res.isEmpty());
}
