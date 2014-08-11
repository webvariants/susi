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
#include "syscall/SysCallController.h"

class SysCallEventInterfaceTest : public ::testing::Test {
protected:

	bool callbackCalled = false;
	std::condition_variable cond;
	std::mutex m;

	void SetUp() override {}
	virtual void TearDown() override {}
};

TEST_F(SysCallEventInterfaceTest, StartProgress) {
	bool test = false;

	Susi::Syscall::Controller controller("./test_config/unified.cfg");

	Susi::once("return_addr_2", [this] (Susi::Event & event) {
		auto stdout = event.payload["stdout"].convert<std::string>();
		auto stderr = event.payload["stderr"].convert<std::string>();
		auto ret = event.payload["return"].convert<int>();

		EXPECT_EQ("TTTTT", stdout);
		EXPECT_EQ("", stderr);
		EXPECT_EQ(0, ret);

		callbackCalled = true;
		cond.notify_one();
	});

	std::map<std::string, std::string> argsReplace;
	argsReplace["arg"] = "TTTTT";

	test = controller.startProcess("return_addr_1","Test",argsReplace);
	EXPECT_FALSE(test);

	test = controller.startProcess("return_addr_2", "ECHO",argsReplace);
	EXPECT_TRUE(test);

	test = controller.startProcess("return_addr_3", "ECHO_BG",argsReplace);
	EXPECT_TRUE(test);

	{
		std::unique_lock<std::mutex> lk(m);
		cond.wait_for(lk,
			std::chrono::duration<int,std::milli>{500},
			[this](){return callbackCalled;});
		EXPECT_TRUE(callbackCalled);
	}
}

/*
TEST_F(SysCallControllerTest, TEST) {

	Susi::Syscall::Worker work1(1, controller.getCmd(), controller.getArgsFor("test"));
	Susi::Syscall::Worker work2(2, controller.getCmd(), controller.getArgsFor("test"));
		
	controller.pool.start(work1);
	controller.pool.start(work2);

	controller.pool.joinAll();
}
*/