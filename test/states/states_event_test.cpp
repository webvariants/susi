#include "gtest/gtest.h"
#include "iocontroller/IOController.h"
#include "world/World.h"
#include "logger/Logger.h"
#include "states/StateController.h"

class StateControllerEventsTest : public ::testing::Test {
protected:
	std::shared_ptr<Susi::States::StateController> controller{nullptr};
	bool callbackCalled = false;
	std::condition_variable cond;
	std::mutex m;
	virtual void SetUp() override {
		world.setupLogger();
		world.setupEventSystem();
		world.setupHeartBeat();
		world.setupIOController();
		world.setupStateController();
		controller = std::shared_ptr<Susi::States::StateController>(world.stateController);
	}
	virtual void TearDown() override {
		world.ioController->deletePath("./states.json");
	}
};

TEST_F(StateControllerEventsTest,setState){
	Susi::Event event("states::setState",Susi::Event::Payload({
		{"key","foo"},
		{"value","bar"}
	}));
	Susi::publish(event,[this](Susi::Event & event){
		EXPECT_TRUE(event.payload.convert<bool>());
		this->callbackCalled = true;
		this->cond.notify_all();
	});
	{
		std::unique_lock<std::mutex> lock(m);
		cond.wait_for(lock,
			std::chrono::duration<int,std::milli>{250},
			[this](){return callbackCalled;});
		EXPECT_TRUE(callbackCalled);		
	}
}

TEST_F(StateControllerEventsTest,setPersistentState){
	Susi::Event event("states::setPersistentState",Susi::Event::Payload({
		{"key","foo"},
		{"value","bar"}
	}));
	Susi::publish(event,[this](Susi::Event & event){
		EXPECT_TRUE(event.payload.convert<bool>());
		this->callbackCalled = true;
		this->cond.notify_all();
	});
	{
		std::unique_lock<std::mutex> lock(m);
		cond.wait_for(lock,
			std::chrono::duration<int,std::milli>{250},
			[this](){return callbackCalled;});
		EXPECT_TRUE(callbackCalled);		
	}
}

TEST_F(StateControllerEventsTest,getState){
	controller->setState("foo","bar");
	Susi::Event event("states::getState",Susi::Event::Payload({
		{"key","foo"}
	}));
	Susi::publish(event,[this](Susi::Event & event){
		EXPECT_EQ("bar",event.payload.convert<std::string>());
		this->callbackCalled = true;
		this->cond.notify_all();
	});
	{
		std::unique_lock<std::mutex> lock(m);
		cond.wait_for(lock,
			std::chrono::duration<int,std::milli>{250},
			[this](){return callbackCalled;});
		EXPECT_TRUE(callbackCalled);		
	}
}


TEST_F(StateControllerEventsTest,getPersistentState){
	controller->setPersistentState("foo","bar");
	Susi::Event event("states::getPersistentState",Susi::Event::Payload({
		{"key","foo"}
	}));
	Susi::publish(event,[this](Susi::Event & event){
		EXPECT_EQ("bar",event.payload.convert<std::string>());
		this->callbackCalled = true;
		this->cond.notify_all();
	});
	{
		std::unique_lock<std::mutex> lock(m);
		cond.wait_for(lock,
			std::chrono::duration<int,std::milli>{250},
			[this](){return callbackCalled;});
		EXPECT_TRUE(callbackCalled);		
	}
}
