/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 * 
 * @author: Tino Rusch (tino.rusch@webvariants.de), Thomas Krause (thomas.krause@webvariants.de)
 */

#include "gtest/gtest.h"
#include "events/global.h"
#include "db/EventInterface.h"
#include "iocontroller/IOController.h"
#include <condition_variable>

#include "world/World.h"


class DBEventInterfaceTest : public ::testing::Test {
	protected:
		std::mutex mutex;
		bool callbackCalledOne = false;
		std::condition_variable condOne;	
		
		void SetUp() override {
			world.setupEventManager();
			world.setupDBManager();
		}
		void TearDown() override {
			Susi::IOController controller;
		    controller.deletePath("./test_sqlite_db_3");
		}
};

using namespace Susi::Events;

TEST_F(DBEventInterfaceTest, Query) {
	
	Susi::Util::Any config{Susi::Util::Any::Array{
        Susi::Util::Any::Array{"test_sqlite_db_3", "sqlite3", "./test_sqlite_db_3"},
        Susi::Util::Any::Array{"test_sqlite_db_4", "sqlite4", "./test_sqlite_db_4"}
    }};

	world.dbManager->init(config);
	
	auto event = createEvent("db::query");
	event->payload =  Susi::Util::Any::Object{
		{"identifier","test_sqlite_db_3"},
		{"query","create table test1 (id integer,name varchar(100));"}
		//{"query","insert into test1(id, name) values(7, \'John\');"}
		//{"query","select id, name from test1 where id = 7;"}			
	};

	publish(std::move(event),[this](SharedEventPtr event){
		EXPECT_NO_THROW ({
			Susi::Util::Any result = event->payload["result"];			
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