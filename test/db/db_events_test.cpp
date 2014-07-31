/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 * 
 * @author: Tino Rusch (tino.rusch@webvariants.de)
 */

#include "gtest/gtest.h"
#include "db/EventInterface.h"
#include "iocontroller/IOController.h"
#include <condition_variable>

#include "world/World.h"
#include "logger/Logger.h"

class DBEventInterfaceTest : public ::testing::Test {
protected:
	void SetUp() override {
		world.setupEventSystem();
		world.setupDBManager();
	}
	void TearDown() override {
		Susi::IOController controller;
	    controller.deletePath("./test_sqlite_db_3");
	}
};

TEST_F(DBEventInterfaceTest, Query) {


	std::vector<std::tuple<std::string,std::string,std::string>> dbs;
    dbs.push_back(std::make_tuple("test_sqlite_db_3", "sqlite3", "./test_sqlite_db_3"));
    dbs.push_back(std::make_tuple("test_sqlite_db_4", "sqlite3", "./test_sqlite_db_4"));
    Poco::Dynamic::Var config(dbs);

	world.dbManager->init(config);
	
	bool callbackCalled = false;
	std::condition_variable cond;
	std::mutex m;

	Susi::once("sqlite_result",[&cond,&callbackCalled](Susi::Event event){
		callbackCalled = true;
		cond.notify_one();
	});

	auto event = Susi::Event("db::query",Susi::Event::Payload({
		{"identifier","test_sqlite_db_3"},
		{"query","create table test1 (id integer,name varchar(100));"}
	}));
	event.returnAddr = "sqlite_result";
	Susi::publish(event);

	{
		std::unique_lock<std::mutex> lk(m);
		cond.wait_for(lk,
			std::chrono::duration<int,std::milli>{250},
			[&callbackCalled](){return callbackCalled;});
		EXPECT_TRUE(callbackCalled);
	}

}