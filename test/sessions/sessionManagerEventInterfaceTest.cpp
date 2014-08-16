/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 *
 * @author: Christian Sonderfeld (christian.sonderfeld@webvariants.de), Thomas Krause (thomas.krause@webvariants.de)
 */

#include <gtest/gtest.h>
#include <condition_variable>
#include <chrono>
//#include <Poco/Dynamic/Var.h>
#include "events/global.h"
#include "sessions/SessionManagerEventInterface.h"
//#include "world/World.h"

using namespace Susi::Sessions;
using namespace Susi::Events;

class SessionManagerEventInterfaceTest : public ::testing::Test {
	protected:
		std::mutex mutex;
		bool callbackCalledOne = false;
		std::condition_variable condOne;
		bool callbackCalledTwo = false;
		std::condition_variable condTwo;
		bool callbackCalledThree = false;
		std::condition_variable condThree;
		bool callbackCalledFour = false;
		std::condition_variable condFour;
		
		void SetUp(){
			world.setupEventManager();
			world.setupHeartBeat();
			world.setupSessionManager();
		}
		void TearDown(){
			world.tearDown();
		}
};


TEST_F(SessionManagerEventInterfaceTest, updateAndCheck) {
	
	auto event = createEvent("session::update");
	event->payload =  Susi::Util::Any::Object{
		{"id", "session1"}, {"seconds", 120}
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

	
	auto event2 = createEvent("session::check");
	event2->payload =  Susi::Util::Any::Object{
		{"id", "session1"}
	};

	publish(std::move(event2),[this](SharedEventPtr event){
		EXPECT_NO_THROW ({
			bool success = event->payload["success"];
			EXPECT_TRUE(success);
		});
		callbackCalledTwo = true;
		condTwo.notify_all();
	});

	{
		std::unique_lock<std::mutex> lk(mutex);
		condTwo.wait_for(lk,std::chrono::milliseconds{100},[this](){return callbackCalledTwo;});
		EXPECT_TRUE(callbackCalledTwo);
	}
}


TEST_F(SessionManagerEventInterfaceTest, setAndgetAttribute) {

	auto event = createEvent("session::setAttribute");
	event->payload =  Susi::Util::Any::Object{
		{"id", "session1"}, {"key", "real"}, {"value", 200}
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

	
	auto event2 = createEvent("session::getAttribute");
	event2->payload =  Susi::Util::Any::Object{
		{"id", "session1"}, {"key", "real"}
	};

	publish(std::move(event2),[this](SharedEventPtr event){
		EXPECT_NO_THROW ({
			Susi::Util::Any value = event->payload["value"];
			int val_int = value;
			EXPECT_EQ(200, val_int);
		});
		callbackCalledTwo = true;
		condTwo.notify_all();
	});

	{
		std::unique_lock<std::mutex> lk(mutex);
		condTwo.wait_for(lk,std::chrono::milliseconds{100},[this](){return callbackCalledTwo;});
		EXPECT_TRUE(callbackCalledTwo);
	}	
}


TEST_F(SessionManagerEventInterfaceTest, pushAttribute) {

	auto event = createEvent("session::pushAttribute");
	event->payload =  Susi::Util::Any::Object{
		{"id", "session1"}, {"key", "real"}, {"value", 100}
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

	auto event2 = createEvent("session::getAttribute");
	event2->payload =  Susi::Util::Any::Object{
		{"id", "session1"}, {"key", "real"}
	};

	publish(std::move(event2),[this](SharedEventPtr event){
		EXPECT_NO_THROW ({
			Susi::Util::Any value = event->payload["value"];
			int val_int = value;
			EXPECT_EQ(100, val_int);
		});
		callbackCalledTwo = true;
		condTwo.notify_all();
	});

	{
		std::unique_lock<std::mutex> lk(mutex);
		condTwo.wait_for(lk,std::chrono::milliseconds{100},[this](){return callbackCalledTwo;});
		EXPECT_TRUE(callbackCalledTwo);
	}	
}


TEST_F(SessionManagerEventInterfaceTest, removeAttribute) {

	auto event = createEvent("session::removeAttribute");
	event->payload =  Susi::Util::Any::Object{
		{"id", "session1"}, {"key", "real"}
	};

	publish(std::move(event),[this](SharedEventPtr event){
		EXPECT_NO_THROW ({
			bool success = event->payload["success"];
			EXPECT_FALSE(success);
		});
		callbackCalledOne = true;
		condOne.notify_all();
	});

	{
		std::unique_lock<std::mutex> lk(mutex);
		condOne.wait_for(lk,std::chrono::milliseconds{100},[this](){return callbackCalledOne;});
		EXPECT_TRUE(callbackCalledOne);
	}

	auto event2 = createEvent("session::setAttribute");
	event2->payload =  Susi::Util::Any::Object{
		{"id", "session1"}, {"key", "real"}, {"value", 200}
	};

	publish(std::move(event2),[this](SharedEventPtr event){
		EXPECT_NO_THROW ({
			bool success = event->payload["success"];
			EXPECT_TRUE(success);
		});
		callbackCalledTwo = true;
		condTwo.notify_all();
	});

	{
		std::unique_lock<std::mutex> lk(mutex);
		condTwo.wait_for(lk,std::chrono::milliseconds{100},[this](){return callbackCalledTwo;});
		EXPECT_TRUE(callbackCalledTwo);
	}

	auto event3 = createEvent("session::removeAttribute");
	event3->payload =  Susi::Util::Any::Object{
		{"id", "session1"}, {"key", "real"}
	};

	publish(std::move(event3),[this](SharedEventPtr event){
		EXPECT_NO_THROW ({
			bool success = event->payload["success"];
			EXPECT_TRUE(success);
		});
		callbackCalledThree = true;
		condThree.notify_all();
	});

	{
		std::unique_lock<std::mutex> lk(mutex);
		condThree.wait_for(lk,std::chrono::milliseconds{100},[this](){return callbackCalledThree;});
		EXPECT_TRUE(callbackCalledThree);
	}

	auto event4 = createEvent("session::getAttribute");
	event4->payload =  Susi::Util::Any::Object{
		{"id", "session1"}, {"key", "real"}
	};

	publish(std::move(event4),[this](SharedEventPtr event){
		EXPECT_NO_THROW ({
			Susi::Util::Any value = event->payload["value"];
			EXPECT_TRUE(value.isNull());
		});
		callbackCalledFour = true;
		condFour.notify_all();
	});

	{
		std::unique_lock<std::mutex> lk(mutex);
		condFour.wait_for(lk,std::chrono::milliseconds{100},[this](){return callbackCalledFour;});
		EXPECT_TRUE(callbackCalledFour);
	}
}