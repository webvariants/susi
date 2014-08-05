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

#include "world/World.h"
#include "logger/Logger.h"

class AuthEventInterfaceTest : public ::testing::Test {
	protected:
		bool callbackCalled = false;
		std::condition_variable cond;
		std::mutex m;

		std::string sessionID = "ljcfbhsdlfsdpf434sdff";

	void SetUp() override {
		world.setupEventSystem();
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

TEST_F(AuthEventInterfaceTest, LogIn_Wrong_User) {
	Susi::once("login_fail_result",[this](Susi::Event & event){
		EXPECT_FALSE(event.payload.convert<bool>());
		callbackCalled = true;
		cond.notify_one();
	});

	auto event = Susi::Event("auth::login",Susi::Event::Payload({
		{"username","FOO"},
		{"password","BAR"}
	}));
	event.returnAddr = "login_fail_result";
	Susi::publish(event);
	{
		std::unique_lock<std::mutex> lk(m);
		cond.wait_for(lk,
			std::chrono::duration<int,std::milli>{500},
			[this](){return callbackCalled;});
		EXPECT_TRUE(callbackCalled);
	}
}

TEST_F(AuthEventInterfaceTest, LogIn_Right_User) {
	Susi::once("login_success_result",[this](Susi::Event & event){
		EXPECT_TRUE(event.payload.convert<bool>());
		callbackCalled = true;
		cond.notify_one();
	});

	auto event = Susi::Event("auth::login",Susi::Event::Payload({
		{"username","John"},
		{"password","Doe"}
	}));
	event.returnAddr = "login_success_result";
	Susi::publish(event);
	{
		std::unique_lock<std::mutex> lk(m);
		cond.wait_for(lk,
			std::chrono::duration<int,std::milli>{500},
			[this](){return callbackCalled;});
		EXPECT_TRUE(callbackCalled);
	}
}

TEST_F(AuthEventInterfaceTest, LogOut) {
	Susi::once("logout_result",[this](Susi::Event & event){
		callbackCalled = true;
		cond.notify_one();
	});

	auto event = Susi::Event("auth::logout",Susi::Event::Payload({
	}));
	event.returnAddr = "logout_result";
	Susi::publish(event);
	{
		std::unique_lock<std::mutex> lk(m);
		cond.wait_for(lk,
			std::chrono::duration<int,std::milli>{500},
			[this](){return callbackCalled;});
		EXPECT_TRUE(callbackCalled);
	}
}

TEST_F(AuthEventInterfaceTest, IsLoggedIn) {
	Susi::once("isloggedin_result",[this](Susi::Event & event){
		callbackCalled = true;
		cond.notify_one();
	});

	auto event = Susi::Event("auth::isLoggedIn",Susi::Event::Payload({
	}));
	event.returnAddr = "isloggedin_result";
	Susi::publish(event);
	{
		std::unique_lock<std::mutex> lk(m);
		cond.wait_for(lk,
			std::chrono::duration<int,std::milli>{500},
			[this](){return callbackCalled;});
		EXPECT_TRUE(callbackCalled);
	}
}

TEST_F(AuthEventInterfaceTest, GetUsername) {
	Susi::once("getusername_result",[this](Susi::Event & event){
		callbackCalled = true;
		cond.notify_one();
	});

	auto event = Susi::Event("auth::getUsername",Susi::Event::Payload({
	}));
	event.returnAddr = "getusername_result";
	Susi::publish(event);
	{
		std::unique_lock<std::mutex> lk(m);
		cond.wait_for(lk,
			std::chrono::duration<int,std::milli>{500},
			[this](){return callbackCalled;});
		EXPECT_TRUE(callbackCalled);
	}
}