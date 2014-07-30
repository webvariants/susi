/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 *
 * @author: Christian Sonderfeld (christian.sonderfeld@webvariants.de)
 */

#include <gtest/gtest.h>
#include <Poco/Dynamic/Var.h>
#include "sessions/SessionManagerEventInterface.h"
#include "world/World.h"

using namespace Susi::Sessions;

class SessionManagerEventInterfaceTest : public ::testing::Test {
protected:
	void SetUp(){
		world.setupHeartBeat();
		world.setupSessionManager();
	}
};

TEST_F(SessionManagerEventInterfaceTest, updateAndCheck) {
	bool callbackCalled = false;
	std::condition_variable cond;
	std::mutex m;
	auto payload = Susi::Event::Payload({{"id", "session1"}, {"seconds", 120}});
	Susi::Event evt("session::update", payload);
	publish(evt, [&cond, &callbackCalled](Susi::Event &event){
		callbackCalled = true;
		cond.notify_all();
		EXPECT_TRUE(event.payload.convert<bool>());
	});
	{
		std::unique_lock<std::mutex> lk(m);
		cond.wait(lk,[&callbackCalled](){return callbackCalled;});
		EXPECT_TRUE(callbackCalled);
	}
	payload = Susi::Event::Payload({{"id", "session1"}});
	evt = Susi::Event("session::check", payload);
	publish(evt, [&cond, &callbackCalled](Susi::Event &event){
		callbackCalled = true;
		cond.notify_all();
		EXPECT_TRUE(event.payload.convert<bool>());
	});
	{
		std::unique_lock<std::mutex> lk(m);
		cond.wait(lk,[&callbackCalled](){return callbackCalled;});
		EXPECT_TRUE(callbackCalled);
	}
}
TEST_F(SessionManagerEventInterfaceTest, setAndgetAttribute) {
	bool callbackCalled = false;
	std::condition_variable cond;
	std::mutex m;
	auto payload = Susi::Event::Payload({{"id", "session1"}, {"key", "real"}, {"value", 200}});
	Susi::Event evt("session::setAttribute", payload);
	publish(evt, [&cond, &callbackCalled](Susi::Event &event){
		callbackCalled = true;
		cond.notify_all();
		EXPECT_TRUE(event.payload.convert<bool>());
	});
	{
		std::unique_lock<std::mutex> lk(m);
		cond.wait(lk,[&callbackCalled](){return callbackCalled;});
		EXPECT_TRUE(callbackCalled);
	}

	payload = Susi::Event::Payload({{"id", "session1"}, {"key", "real"}});
	evt = Susi::Event("session::getAttribute", payload);
	publish(evt, [&cond, &callbackCalled](Susi::Event &event){
		callbackCalled = true;
		cond.notify_all();
		EXPECT_EQ(200, event.payload.convert<int>());
	});
	{
		std::unique_lock<std::mutex> lk(m);
		cond.wait(lk,[&callbackCalled](){return callbackCalled;});
		EXPECT_TRUE(callbackCalled);
	}
}

TEST_F(SessionManagerEventInterfaceTest, pushAttribute) {
	bool callbackCalled = false;
	std::condition_variable cond;
	std::mutex m;
	auto payload = Susi::Event::Payload({{"id", "session1"}, {"key", "real"}, {"value", 100}});
	Susi::Event evt("session::pushAttribute", payload);
	publish(evt, [&cond, &callbackCalled](Susi::Event &event){
		callbackCalled = true;
		cond.notify_all();
		EXPECT_TRUE(event.payload.convert<bool>());
	});
	{
		std::unique_lock<std::mutex> lk(m);
		cond.wait(lk,[&callbackCalled](){return callbackCalled;});
		EXPECT_TRUE(callbackCalled);
	}

	payload = Susi::Event::Payload({{"id", "session1"}, {"key", "real"}});
	evt.topic = "session::getAttribute";
	evt.payload = payload;
	publish(evt, [&cond, &callbackCalled](Susi::Event &event){
		callbackCalled = true;
		cond.notify_all();
		auto ret = event.payload;
		EXPECT_EQ(100, ret[0].convert<int>());
	});
	{
		std::unique_lock<std::mutex> lk(m);
		cond.wait(lk,[&callbackCalled](){return callbackCalled;});
		EXPECT_TRUE(callbackCalled);
	}
}

TEST_F(SessionManagerEventInterfaceTest, removeAttribute) {
	bool callbackCalled = false;
	std::condition_variable cond;
	std::mutex m;
	auto payload = Susi::Event::Payload({{"id", "session1"}, {"key", "real"}});
	Susi::Event evt("session::removeAttribute", payload);
	publish(evt, [&cond, &callbackCalled](Susi::Event &event){
		callbackCalled = true;
		cond.notify_all();
		EXPECT_FALSE(event.payload.convert<bool>());
	});
	{
		std::unique_lock<std::mutex> lk(m);
		cond.wait(lk,[&callbackCalled](){return callbackCalled;});
		EXPECT_TRUE(callbackCalled);
	}

	payload = Susi::Event::Payload({{"id", "session1"}, {"key", "real"}, {"value", 200}});
	Susi::Event evt1("session::setAttribute", payload);
	Susi::publish(evt1);

	payload = Susi::Event::Payload({{"id", "session1"}, {"key", "real"}});
	Susi::Event evt2("session::removeAttribute", payload);
	publish(evt2, [&cond, &callbackCalled](Susi::Event &event){
		callbackCalled = true;
		cond.notify_all();
		EXPECT_TRUE(event.payload.convert<bool>());
	});
	{
		std::unique_lock<std::mutex> lk(m);
		cond.wait(lk,[&callbackCalled](){return callbackCalled;});
		EXPECT_TRUE(callbackCalled);
	}

	payload = Susi::Event::Payload({{"id", "session1"}, {"key", "real"}});
	evt = Susi::Event("session::getAttribute", payload);
	publish(evt, [&cond, &callbackCalled](Susi::Event &event){
		callbackCalled = true;
		cond.notify_all();
		EXPECT_TRUE(event.payload.isEmpty());
	});
	{
		std::unique_lock<std::mutex> lk(m);
		cond.wait(lk,[&callbackCalled](){return callbackCalled;});
		EXPECT_TRUE(callbackCalled);
	}
}
