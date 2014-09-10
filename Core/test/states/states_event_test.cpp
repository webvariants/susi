/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 *
 * @author: Thomas Krause (thomas.krause@webvariants.de)
 */

#include "gtest/gtest.h"
#include "events/global.h"
#include <condition_variable>
#include <chrono>

#include "states/StateEventInterface.h"
#include "iocontroller/IOController.h"

class StateEventsTest : public ::testing::Test {
	protected:
		std::shared_ptr<Susi::States::StateController> controller{nullptr};
		std::mutex mutex;
		bool callbackCalledOne = false;
		std::condition_variable condOne;
		bool callbackCalledTwo = false;
		std::condition_variable condTwo;
	
	virtual void SetUp() override {
		world.setupEventManager();
		//world.setupHeartBeat();
		world.setupIOController();
		world.setupStateController();
		controller = std::shared_ptr<Susi::States::StateController>(world.stateController);
	}
	virtual void TearDown() override {
		world.ioController->deletePath("./states.json");
	}
};

using namespace Susi::Events;

TEST_F(StateEventsTest,setState){
	auto event = createEvent("state::setState");
	event->payload =  Susi::Util::Any::Object{
		{"stateID","foo"},
		{"value","bar"}
	};

	publish(std::move(event),[this](SharedEventPtr event){
		EXPECT_NO_THROW ({
			bool success = event->payload["success"];
			EXPECT_TRUE(success);
		});
		callbackCalledOne = true;
		condOne.notify_all();
	});

	{
		std::unique_lock<std::mutex> lk(mutex);
		condOne.wait_for(lk,std::chrono::milliseconds{100},[this](){return callbackCalledOne;});
		EXPECT_TRUE(callbackCalledOne);
	}
}

TEST_F(StateEventsTest,setPersistentState){
	auto event = createEvent("state::setPersistentState");
	event->payload =  Susi::Util::Any::Object{
		{"stateID","foo"},
		{"value","bar"}
	};

	publish(std::move(event),[this](SharedEventPtr event){
		EXPECT_NO_THROW ({
			bool success = event->payload["success"];
			EXPECT_TRUE(success);
		});
		callbackCalledOne = true;
		condOne.notify_all();
	});

	{
		std::unique_lock<std::mutex> lk(mutex);
		condOne.wait_for(lk,std::chrono::milliseconds{100},[this](){return callbackCalledOne;});
		EXPECT_TRUE(callbackCalledOne);
	}
}

TEST_F(StateEventsTest,getState){
	controller->setState("foo","bar");

	auto event = createEvent("state::getState");
	event->payload =  Susi::Util::Any::Object{
		{"stateID","foo"},
	};

	publish(std::move(event),[this](SharedEventPtr event){
		EXPECT_NO_THROW ({
			Susi::Util::Any value = event->payload["value"];
			std::string val = value;
			EXPECT_EQ("bar", val);
		});
		callbackCalledOne = true;
		condOne.notify_all();
	});

	{
		std::unique_lock<std::mutex> lk(mutex);
		condOne.wait_for(lk,std::chrono::milliseconds{100},[this](){return callbackCalledOne;});
		EXPECT_TRUE(callbackCalledOne);
	}
}

TEST_F(StateEventsTest,getPersistentState){
	controller->setPersistentState("foo","bar");

	auto event = createEvent("state::getPersistentState");
	event->payload =  Susi::Util::Any::Object{
		{"stateID","foo"},
	};

	publish(std::move(event),[this](SharedEventPtr event){
		EXPECT_NO_THROW ({
			Susi::Util::Any value = event->payload["value"];
			std::string val = value;
			EXPECT_EQ("bar", val);
		});
		callbackCalledOne = true;
		condOne.notify_all();
	});

	{
		std::unique_lock<std::mutex> lk(mutex);
		condOne.wait_for(lk,std::chrono::milliseconds{100},[this](){return callbackCalledOne;});
		EXPECT_TRUE(callbackCalledOne);
	}
}