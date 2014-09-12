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
#include "auth/AuthEventInterface.h"
#include <condition_variable>

//#include "world/World.h"
#include "events/global.h"
//#include "logger/Logger.h"

/*class AuthEventInterfaceTest : public ::testing::Test {
	protected:
		std::mutex mutex;
		bool callbackCalledOne = false;
		std::condition_variable condOne;
		bool callbackCalledTwo = false;
		std::condition_variable condTwo;

		std::string sessionID = "ljcfbhsdlfsdpf434sdff";

	void SetUp() override {
		world.setupEventManager();
		world.setupIOController();
		world.setupSessionManager();
		world.setupDBManager();
		world.setupAuthController();
		
		auto db = world.dbManager->getDatabase("auth");

		db->query("create table users("
        "    id integer,"
        "    username varchar(100),"
        "    password varchar(100)"
        ");");

        db->query("insert into users(id, username, password) values(7, \'John\', \'Doe\');");
	}

	virtual void TearDown() override {
		world.ioController->deletePath("./auth.db");
	}
};

using namespace Susi::Events;

TEST_F(AuthEventInterfaceTest, LogIn_Wrong_User) {

	auto event = createEvent("auth::login");
	event->payload =  Susi::Util::Any::Object{
		{"username","FOO"},
		{"password","BAR"}
	};
	event->sessionID = sessionID;


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
}

TEST_F(AuthEventInterfaceTest, LogIn_Right_User) {

	auto event = createEvent("auth::login");
	event->payload =  Susi::Util::Any::Object{
		{"username","John"},
		{"password","Doe"}
	};
	event->sessionID = sessionID;

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

TEST_F(AuthEventInterfaceTest, LogOut) {

	auto event = createEvent("auth::logout");
	event->sessionID = sessionID;
	
	publish(std::move(event),[this](SharedEventPtr event){			
		callbackCalledOne = true;
		condOne.notify_all();
	});

	{
		std::unique_lock<std::mutex> lk(mutex);
		condOne.wait_for(lk,std::chrono::milliseconds{100},[this](){return callbackCalledOne;});
		EXPECT_TRUE(callbackCalledOne);
	}	
}

TEST_F(AuthEventInterfaceTest, IsLoggedIn) {


	auto event = createEvent("auth::login");
	event->payload =  Susi::Util::Any::Object{
		{"username","John"},
		{"password","Doe"}
	};
	event->sessionID = sessionID;

	publish(std::move(event),[this](SharedEventPtr event){


		auto evt = createEvent("auth::isLoggedIn");
		evt->sessionID = event->sessionID;
		publish(std::move(evt),[this](SharedEventPtr event){
			EXPECT_NO_THROW ({
				bool success = event->payload["success"];
				EXPECT_TRUE(success);
			});
			callbackCalledTwo = true;
			condTwo.notify_all();
		});


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
	{
		std::unique_lock<std::mutex> lk(mutex);
		condTwo.wait_for(lk,std::chrono::milliseconds{100},[this](){return callbackCalledTwo;});
		EXPECT_TRUE(callbackCalledTwo);
	}
}

TEST_F(AuthEventInterfaceTest, GetUsername) {

	auto event = createEvent("auth::login");
	event->payload =  Susi::Util::Any::Object{
		{"username","John"},
		{"password","Doe"}
	};
	event->sessionID = sessionID;

	publish(std::move(event),[this](SharedEventPtr event){

		auto evt = createEvent("auth::getUsername");
		evt->sessionID = event->sessionID;
		publish(std::move(evt),[this](SharedEventPtr event){
			EXPECT_NO_THROW ({
				std::string username = event->payload["username"];
				EXPECT_EQ("John", username);
			});
			callbackCalledTwo = true;
			condTwo.notify_all();
		});


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
	{
		std::unique_lock<std::mutex> lk(mutex);
		condTwo.wait_for(lk,std::chrono::milliseconds{100},[this](){return callbackCalledTwo;});
		EXPECT_TRUE(callbackCalledTwo);
	}
}*/