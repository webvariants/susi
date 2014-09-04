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

#include "iocontroller/IOEventInterface.h"

class IOEventInterfaceTest : public ::testing::Test {
	protected:
		std::mutex mutex;
		bool callbackCalledOne = false;
		std::condition_variable condOne;
		bool callbackCalledTwo = false;
		std::condition_variable condTwo;

		void SetUp() override {
			world.setupEventManager();
			world.setupIOController();			
			world.ioController->makeDir("./IO_EVENT_TESTS/CHECKDIR/");
		}
		virtual void TearDown() override {
			world.ioController->deletePath("./IO_EVENT_TESTS/");
		}
};

using namespace Susi::Events;

TEST_F(IOEventInterfaceTest, WriteFile) {
	
	auto event = createEvent("io::writeFile");
	event->payload =  Susi::Util::Any::Object{
		{"filename","./IO_EVENT_TESTS/write.txt"},
		{"content","foo bar"}
	};

	publish(std::move(event),[this](SharedEventPtr event){
		EXPECT_NO_THROW ({
			int bytesWritten = event->payload["bytesWritten"];
			EXPECT_EQ(7,bytesWritten);
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

TEST_F(IOEventInterfaceTest, WriteFileInvalidDirectory) {
	
	auto event = createEvent("io::writeFile");
	event->payload =  Susi::Util::Any::Object{
		{"filename","./IO_EVENT_TESTS/BLAA/write.txt"},
		{"content","foo bar"}
	};

	publish(std::move(event),[this](SharedEventPtr event){
		EXPECT_EQ(size_t{1},event->headers.size());
		EXPECT_EQ("error", event->headers[0].first);
		EXPECT_EQ("Error in handleWriteFile(): WriteFile: Dir don't exists!./IO_EVENT_TESTS/BLAA/write.txt", event->headers[0].second);
		
		callbackCalledOne = true;
		condOne.notify_all();
	});

	{
		std::unique_lock<std::mutex> lk(mutex);
		condOne.wait_for(lk,std::chrono::milliseconds{500},[this](){return callbackCalledOne;});
		EXPECT_TRUE(callbackCalledOne);
	}
}

TEST_F(IOEventInterfaceTest, ReadFile) {
	world.ioController->writeFile("./IO_EVENT_TESTS/read.txt","foobar");

	auto event = createEvent("io::readFile");
	event->payload =  Susi::Util::Any::Object{
		{"filename","./IO_EVENT_TESTS/read.txt"}
	};

	publish(std::move(event),[this](SharedEventPtr event){
		EXPECT_NO_THROW ({
			std::string content = event->payload["content"];
			EXPECT_EQ("foobar",content);
		});

		callbackCalledOne = true;
		condOne.notify_all();
	});

	{
		std::unique_lock<std::mutex> lk(mutex);
		condOne.wait_for(lk,std::chrono::milliseconds{500},[this](){return callbackCalledOne;});
		EXPECT_TRUE(callbackCalledOne);
	}
}

TEST_F(IOEventInterfaceTest, DeletePath) {

	world.ioController->writeFile("./IO_EVENT_TESTS/delete.txt","TEXT TO DELETE");

	auto event = createEvent("io::deletePath");
	event->payload =  Susi::Util::Any::Object{
		{"path","./IO_EVENT_TESTS/delete.txt"}
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
		condOne.wait_for(lk,std::chrono::milliseconds{500},[this](){return callbackCalledOne;});
		EXPECT_TRUE(callbackCalledOne);
	}
}

TEST_F(IOEventInterfaceTest, MakeDir) {

	auto event = createEvent("io::makeDir");
	event->payload =  Susi::Util::Any::Object{
		{"dir","./IO_EVENT_TESTS/test_dir"}
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
		condOne.wait_for(lk,std::chrono::milliseconds{500},[this](){return callbackCalledOne;});
		EXPECT_TRUE(callbackCalledOne);
	}
}

TEST_F(IOEventInterfaceTest, MovePath) {

	auto event = createEvent("io::movePath");
	event->payload =  Susi::Util::Any::Object{
		{"source_path","./IO_EVENT_TESTS/CHECKDIR/"},
		{"dest_path","./IO_EVENT_TESTS/MOVED_CHECKDIR"}
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
		condOne.wait_for(lk,std::chrono::milliseconds{500},[this](){return callbackCalledOne;});
		EXPECT_TRUE(callbackCalledOne);
	}
}


TEST_F(IOEventInterfaceTest, CopyPath) {

	auto event = createEvent("io::copyPath");
	event->payload =  Susi::Util::Any::Object{
		{"source_path","./IO_EVENT_TESTS/CHECKDIR/"},
		{"dest_path","./IO_EVENT_TESTS/MOVED_CHECKDIR"}
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
		condOne.wait_for(lk,std::chrono::milliseconds{500},[this](){return callbackCalledOne;});
		EXPECT_TRUE(callbackCalledOne);
	}
}