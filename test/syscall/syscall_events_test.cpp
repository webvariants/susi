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

#include "syscall/SysCallEventInterface.h"
#include "iocontroller/IOController.h"

class SysCallEventInterfaceTest : public ::testing::Test {
	protected:
		const std::string TMP_PATH    = "./syscall_test/";	
		const std::string CONFIG_PATH = "./test_config/";	
		Susi::IOController io;
		
		std::mutex mutex;
		bool callbackCalledOne = false;
		std::condition_variable condOne;
		bool callbackCalledTwo = false;
		std::condition_variable condTwo;
		bool callbackCalledThree = false;
		std::condition_variable condThree;

	virtual void SetUp() override {		
		io.makeDir(CONFIG_PATH);
		std::string config = CONFIG_PATH+"unified.cfg";
		io.writeFile(config,"{\"ECHO\": {\"cmd\": \"/bin/echo\",\"args\": \"-n $arg\",\"background\": false	},\"INBG\": {\"cmd\": \"/bin/bash\", \"args\": \"/home/thomas/GITHUB/susi/syscall_test/test.sh\", \"background\": true } }");

		io.makeDir(TMP_PATH);
		std::string script = TMP_PATH+"test.sh";
		std::string output = TMP_PATH+"test.out";
		io.writeFile(script,"#!/bin/bash\necho -n foobar > "+output+"\nexit 0\n");
		io.setExecutable(script,true);

		world.setupEventManager();
		world.setupSysCallController();
	}

	virtual void TearDown() override {
		io.deletePath(TMP_PATH);
		io.deletePath(CONFIG_PATH);
	}
};

using namespace Susi::Events;

TEST_F(SysCallEventInterfaceTest,StartProgress){

	Susi::Events::subscribe("syscall::endProcess",[this](Susi::Events::EventPtr event){

		Susi::Util::Any result = event->payload["result"];
		int returnVal   = result["return"];
		std::string err = result["stderr"];
		std::string out = result["stdout"];
		std::string process_type = result["process_type"];

		EXPECT_EQ(0, returnVal);
		EXPECT_EQ("", err);
		EXPECT_EQ("TTT", out);
		EXPECT_EQ("ECHO", process_type);
		callbackCalledTwo = true;
		condTwo.notify_all();

		Susi::Events::ack(std::move(event));
	});
	
	auto event = createEvent("syscall::startProcess");
	event->payload =  Susi::Util::Any::Object{		
		{"process_type","ECHO"},
		{"argsReplace" , Susi::Util::Any::Object{{"arg", "TTT"}}}
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

	{
		std::unique_lock<std::mutex> lk(mutex);
		condTwo.wait_for(lk,std::chrono::milliseconds{500},[this](){return callbackCalledTwo;});
		EXPECT_TRUE(callbackCalledTwo);
	}
}

TEST_F(SysCallEventInterfaceTest,StartProgressBackground){

	Susi::Events::subscribe("syscall::endProcess",[this](Susi::Events::EventPtr event){

		Susi::Util::Any result = event->payload["result"];
		int returnVal   = result["return"];
		std::string err = result["stderr"];
		std::string out = result["stdout"];
		std::string process_type = result["process_type"];

		EXPECT_EQ(0, returnVal);
		EXPECT_EQ("", err);
		EXPECT_EQ("", out);
		EXPECT_EQ("INBG", process_type);
		callbackCalledThree = true;
		condThree.notify_all();

		Susi::Events::ack(std::move(event));
	});

	Susi::Events::subscribe("syscall::startedProcess",[this](Susi::Events::EventPtr event){

		Susi::Util::Any result = event->payload["result"];
		bool started   = result["started"];
		std::string process_type = result["process_type"];
		
		EXPECT_TRUE(started);
		EXPECT_EQ("INBG", process_type);

		callbackCalledTwo = true;
		condTwo.notify_all();

		Susi::Events::ack(std::move(event));
	});
	
	auto event = createEvent("syscall::startProcess");
	event->payload =  Susi::Util::Any::Object{
		{"process_type","INBG"},
		{"argsReplace" , Susi::Util::Any::Object{{}}}
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

	{
		std::unique_lock<std::mutex> lk(mutex);
		condTwo.wait_for(lk,std::chrono::milliseconds{500},[this](){return callbackCalledTwo;});
		EXPECT_TRUE(callbackCalledTwo);
	}

	{
		std::unique_lock<std::mutex> lk(mutex);
		condThree.wait_for(lk,std::chrono::milliseconds{500},[this](){return callbackCalledThree;});
		EXPECT_TRUE(callbackCalledThree);
	}
}

TEST_F(SysCallEventInterfaceTest,StartWrongProgress){

	auto event = createEvent("syscall::startProcess");
	event->payload =  Susi::Util::Any::Object{
		{"process_type","DONT EXIST IN CONFIG"},
		{"argsReplace" , Susi::Util::Any::Object{{"arg", "TTT"}}}
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
}