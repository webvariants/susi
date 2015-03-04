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


#include "susi/util/ComponentTest.h"

class SyscallComponentTest : public ComponentTest {
protected:
	
	virtual void SetUp() override {
		componentManager->startComponent("syscallcontroller");
	}
	
	virtual void TearDown() override {
		componentManager->stopComponent("syscallcontroller");
	}

	virtual void GoodCases() override {

		auto evt1 = createEvent("syscall::exec");
		evt1->payload["cmd"] = "TEST1";		
		auto result1 = publish_sync(std::move(evt1));
		//std::cout<<"EDGE CASE:"<<result1->toAny().toJSONString()<<std::endl;

		int ret1 = result1->payload["return"];
		std::string stderr1 = result1->payload["stderr"];
		std::string stdout1 = result1->payload["stdout"];

		EXPECT_EQ(0, ret1);
		EXPECT_EQ("", stderr1);
		EXPECT_EQ("foo", stdout1);

		auto evt2 = createEvent("syscall::exec");
		evt2->payload["cmd"] = "TEST2";
		auto result2 = publish_sync(std::move(evt2));
		
		int ret2 = result2->payload["return"];
		std::string stderr2 = result2->payload["stderr"];
		std::string stdout2 = result2->payload["stdout"];

		EXPECT_EQ(0, ret2);
		EXPECT_EQ("foo", stderr2);
		EXPECT_EQ("", stdout2);

		auto evt3 = createEvent("syscall::exec");
		evt3->payload["cmd"] = "TEST3";
		evt3->payload["args"] = Susi::Util::Any::Object{{"arg" , "foo"}};		
		auto result3 = publish_sync(std::move(evt3));

		int ret3 = result3->payload["return"];
		std::string stderr3 = result3->payload["stderr"];
		std::string stdout3 = result3->payload["stdout"];

		EXPECT_EQ(0, ret3);
		EXPECT_EQ("", stderr3);
		EXPECT_EQ("foo", stdout3);
		
	}

	virtual void BadCases() override {

		auto evt1 = createEvent("syscall::exec");
		evt1->payload["cmd"] = "TEST_DONT_EXIST_IN_CONFIG";
		auto result1 = publish_sync(std::move(evt1));		

		EXPECT_TRUE(hasErrorHeader(result1));	
	}

	virtual void EdgeCases() override {
		// missing cmd param
		auto evt2 = createEvent("syscall::exec");		
		auto result2 = publish_sync(std::move(evt2));
		EXPECT_TRUE(hasErrorHeader(result2));		
	}

};

TEST_F(SyscallComponentTest,GoodCases){
	GoodCases();
}

TEST_F(SyscallComponentTest,BadCases){
	BadCases();
}

TEST_F(SyscallComponentTest,EdgeCases){
	EdgeCases();
}
