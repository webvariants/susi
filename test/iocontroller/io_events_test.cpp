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
		bool callbackCalled = false;
		std::condition_variable cond;
		std::mutex m;

		void SetUp() override {
			//world.setupLogger();
			//Susi::setLogLevel(Susi::Logger::ALL);
			world.setupIOController();
			world.ioController->makeDir("./IO_EVENT_TESTS/CHECKDIR/");
		}
		virtual void TearDown() override {
			world.ioController->deletePath("./IO_EVENT_TESTS/");
		}
};
/*
#include <gtest/gtest.h>
#include <Poco/Dynamic/Var.h>
#include "iocontroller/IOEventInterface.h"

#include "world/World.h"

class IOEventInterfaceTest : public ::testing::Test {
	protected:
		bool callbackCalled = false;
		std::condition_variable cond;
		std::mutex m;

		void SetUp() override {
			//world.setupLogger();
			//Susi::setLogLevel(Susi::Logger::ALL);
			world.setupIOController();
			world.ioController->makeDir("./IO_EVENT_TESTS/CHECKDIR/");
		}
		virtual void TearDown() override {
			world.ioController->deletePath("./IO_EVENT_TESTS/");
		}
};


TEST_F(IOEventInterfaceTest, WriteFile) {
	
	Susi::once("file_write_result",[this](Susi::Event & event){
		EXPECT_TRUE(event.payload.convert<bool>());
		callbackCalled = true;
		cond.notify_one();
	});

	auto event = Susi::Event("io::writeFile",Susi::Event::Payload({
		{"filename","./IO_EVENT_TESTS/write.txt"},
		{"content","foo bar"}
	}));
	event.returnAddr = "file_write_result";
	Susi::publish(event);
	{
		std::unique_lock<std::mutex> lk(m);
		cond.wait_for(lk,
			std::chrono::duration<int,std::milli>{500},
			[this](){return callbackCalled;});
		EXPECT_TRUE(callbackCalled);
	}
}

TEST_F(IOEventInterfaceTest, WriteFileInvalidDirectory) {
	
	Susi::once("file_write_invalid_result",[this](Susi::Event & event){
		EXPECT_FALSE(event.payload.convert<bool>());
		callbackCalled = true;
		cond.notify_one();
	});

	auto event = Susi::Event("io::writeFile",Susi::Event::Payload({
		{"filename","./IO_EVENT_TESTS/BLAA/write.txt"},
		{"content","foo bar"}
	}));
	event.returnAddr = "file_write_invalid_result";
	Susi::publish(event);

	{
		std::unique_lock<std::mutex> lk(m);
		cond.wait_for(lk,
			std::chrono::duration<int,std::milli>{500},
			[this](){return callbackCalled;});
		EXPECT_TRUE(callbackCalled);
	}
}

TEST_F(IOEventInterfaceTest, ReadFile) {
	world.ioController->writeFile("./IO_EVENT_TESTS/read.txt","foobar");

	Susi::once("file_read_result",[this](Susi::Event & event){
		EXPECT_EQ("foobar",event.payload.convert<std::string>());
		callbackCalled = true;
		cond.notify_one();
	});

	auto event = Susi::Event("io::readFile",Susi::Event::Payload({
		{"filename","./IO_EVENT_TESTS/read.txt"},
	}));
	event.returnAddr = "file_read_result";
	Susi::publish(event);

	{
		std::unique_lock<std::mutex> lk(m);
		cond.wait_for(lk,
			std::chrono::duration<int,std::milli>{500},
			[this](){return callbackCalled;});
		EXPECT_TRUE(callbackCalled);
	}
}

TEST_F(IOEventInterfaceTest, DeletePath) {

	world.ioController->writeFile("./IO_EVENT_TESTS/delete.txt","TEXT TO DELETE");

	Susi::once("file_delete_result",[this](Susi::Event & event){
		callbackCalled = true;
		cond.notify_one();
	});

	auto event = Susi::Event("io::deletePath",Susi::Event::Payload({
		{"path","./IO_EVENT_TESTS/delete.txt"},
	}));
	event.returnAddr = "file_delete_result";
	Susi::publish(event);

	{
		std::unique_lock<std::mutex> lk(m);
		cond.wait_for(lk,
			std::chrono::duration<int,std::milli>{500},
			[this](){return callbackCalled;});
		EXPECT_TRUE(callbackCalled);
	}
}

TEST_F(IOEventInterfaceTest, MakeDir) {
	Susi::once("make_dir_result",[this](Susi::Event event){
		EXPECT_TRUE(event.payload.convert<bool>());
		callbackCalled = true;
		cond.notify_one();
	});

	auto event = Susi::Event("io::makeDir",Susi::Event::Payload({
		{"dir","./IO_EVENT_TESTS/test_dir"},
	}));
	event.returnAddr = "make_dir_result";
	Susi::publish(event);

	{
		std::unique_lock<std::mutex> lk(m);
		cond.wait_for(lk,
			std::chrono::duration<int,std::milli>{500},
			[this](){return callbackCalled;});
		EXPECT_TRUE(callbackCalled);
	}
}

TEST_F(IOEventInterfaceTest, MovePath) {
	Susi::once("move_path_result",[this](Susi::Event & event){
		EXPECT_TRUE(event.payload.convert<bool>());
		callbackCalled = true;
		cond.notify_one();
	});

	auto event = Susi::Event("io::movePath",Susi::Event::Payload({
		{"source_path","./IO_EVENT_TESTS/CHECKDIR/"},
		{"dest_path","./IO_EVENT_TESTS/MOVED_CHECKDIR"},
	}));
	event.returnAddr = "move_path_result";
	Susi::publish(event);

	{
		std::unique_lock<std::mutex> lk(m);
		cond.wait_for(lk,
			std::chrono::duration<int,std::milli>{500},
			[this](){return callbackCalled;});
		EXPECT_TRUE(callbackCalled);
	}
}

TEST_F(IOEventInterfaceTest, CopyPath) {
	Susi::once("copy_path_result",[this](Susi::Event & event){
		EXPECT_TRUE(event.payload.convert<bool>());
		callbackCalled = true;
		cond.notify_one();
	});

	auto event = Susi::Event("io::copyPath",Susi::Event::Payload({
		{"source_path","./IO_EVENT_TESTS/CHECKDIR/"},
		{"dest_path","./IO_EVENT_TESTS/MOVED_CHECKDIR"},
	}));
	event.returnAddr = "copy_path_result";
	Susi::publish(event);

	{
		std::unique_lock<std::mutex> lk(m);
		cond.wait_for(lk,
			std::chrono::duration<int,std::milli>{500},
			[this](){return callbackCalled;});
		EXPECT_TRUE(callbackCalled);
	}
}
*/