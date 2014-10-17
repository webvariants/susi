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

#include "util/ComponentTest.h"

class AuthControllerComponentTest : public ComponentTest {

public:
	//on live system, sessionID will be set by handlePublish from ApiServerForComponent
	std::string sessionID = "abc";
protected:

	virtual void SetUp() override {
		componentManager->startComponent("authcontroller");
		auto authController = componentManager->getComponent<Susi::Auth::Controller>("authcontroller");
		authController->addUser("John","Doe",1);
	}

	virtual void TearDown() override {
		componentManager->stopComponent("authcontroller");
	}

	virtual void GoodCases() override {
		//create event
		auto evt = createEvent("auth::login");
		evt->payload["username"] = "John";
		evt->payload["password"] = "Doe";
		evt->sessionID = sessionID;
		//fire event
		auto result = publish_sync(std::move(evt));
		//check event
		EXPECT_TRUE(static_cast<bool>(result->payload["success"]));

		auto evt2 = createEvent("auth::isLoggedIn");
		evt2->sessionID = sessionID;
		auto result2 = publish_sync(std::move(evt2));
		EXPECT_TRUE(static_cast<bool>(result2->payload["success"]));

		auto evt3 = createEvent("auth::getUsername");
		evt3->sessionID = sessionID;
		auto result3 = publish_sync(std::move(evt3));
		EXPECT_EQ("John", static_cast<std::string>(result3->payload["username"]));

		auto evt4 = createEvent("auth::logout");
		evt4->sessionID = sessionID;
		auto result4 = publish_sync(std::move(evt4));

		auto evt5 = createEvent("auth::isLoggedIn");
		evt5->sessionID = sessionID;
		auto result5 = publish_sync(std::move(evt5));
		EXPECT_FALSE(static_cast<bool>(result5->payload["success"]));

	}

	virtual void BadCases() override {
		//create event
		auto evt = createEvent("auth::login");
		evt->payload["username"] = "foo";
		evt->payload["password"] = "bar";
		evt->sessionID = sessionID;
		//fire event
		auto result = publish_sync(std::move(evt));
		//check event
		EXPECT_EQ(Susi::Util::Any{false}, result->payload["success"]);

		// expect empty username
		auto evt6 = createEvent("auth::getUsername");
		evt6->sessionID = sessionID;
		auto result6 = publish_sync(std::move(evt6));
		EXPECT_EQ("", static_cast<std::string>(result6->payload["username"]));


		auto evt2 = createEvent("auth::login");
		evt2->payload["username"] = "John";
		evt2->payload["password"] = "Doe";
		evt2->sessionID = sessionID;
		publish_sync(std::move(evt2));

		// logout with wrong sessionID
		auto evt3 = createEvent("auth::logout");
		evt3->sessionID = sessionID + "foo" ;
		auto result3 = publish_sync(std::move(evt3));

		EXPECT_EQ(Susi::Util::Any{false}, result3->payload["success"]);

		auto evt4 = createEvent("auth::isLoggedIn");
		evt4->sessionID = sessionID;
		auto result4 = publish_sync(std::move(evt4));
		EXPECT_TRUE(static_cast<bool>(result4->payload["success"]));


		// logout with mising payload
		auto evt5 = createEvent("auth::logout");
		auto result5 = publish_sync(std::move(evt5));
		EXPECT_FALSE(static_cast<bool>(result5->payload["success"]));	
	}

	virtual void EdgeCases() override {
		// missing param
		auto evt = createEvent("auth::login");
		evt->payload["username"] = "foo";
		evt->sessionID = sessionID;
		auto result = publish_sync(std::move(evt));

		//std::cout<<"EDGE CASE:"<<result->toAny().toJSONString()<<std::endl;
		EXPECT_FALSE(static_cast<bool>(result->payload["success"]));			
		
		EXPECT_TRUE(hasErrorHeader(result));
	}

};

TEST_F(AuthControllerComponentTest,GoodCases){
	GoodCases();
}

TEST_F(AuthControllerComponentTest,BadCases){
	BadCases();
}

TEST_F(AuthControllerComponentTest,EdgeCases){
	EdgeCases();
}
