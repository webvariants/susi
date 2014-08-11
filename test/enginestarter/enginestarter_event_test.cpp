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

#include "enginestarter/EngineEventinterface.h"

class EngineEventsTest : public ::testing::Test {
	protected:
		std::shared_ptr<Susi::States::StateController> controller{nullptr};
		std::mutex mutex;
		bool callbackCalledOne = false;
		std::condition_variable condOne;
		bool callbackCalledTwo = false;
		std::condition_variable condTwo;

		const std::string TMP_PATH = "./enginestarter_test/";
		Susi::IOController io;
	
	virtual void SetUp() override {
		world.setupEventManager();
		world.setupEngineStarter();	

		io.makeDir(TMP_PATH);

		std::string script = TMP_PATH+"test.sh";
		std::string output = TMP_PATH+"test.out";

		io.writeFile(script,"#!/bin/bash\nfor i in $(seq 1 100); do echo -n foobar >> "+output+"\nsleep 0.1; done; exit 0\n");
		io.setExecutable(script,true);	
	}
	virtual void TearDown() override {
		io.deletePath(TMP_PATH);
	}
};

using namespace Susi::Events;

TEST_F(EngineEventsTest, Start){	
	auto event = createEvent("enginestarter::start");	

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

TEST_F(EngineEventsTest, Restart){	
	auto event = createEvent("enginestarter::restart");	

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

TEST_F(EngineEventsTest, Stop){
	auto event = createEvent("enginestarter::stop");	

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