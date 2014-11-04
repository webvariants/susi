#include "util/ComponentTest.h"
#include <iostream>

class SessionManagerComponentTest : public ComponentTest {
protected:
	std::string sessionID{"supertestsession"};
	virtual void SetUp() override {
		componentManager->startComponent("sessionmanager");
	}

	virtual void TearDown() override {
		componentManager->stopComponent("sessionmanager");
	}

	virtual void GoodCases() override {}
	virtual void BadCases() override {}
	virtual void EdgeCases() override {}

};

TEST_F(SessionManagerComponentTest, checkSessions) {

	//create event
	auto evt = createEvent("session::update");
	evt->sessionID = sessionID;
	evt->payload["seconds"] = 120;
	//fire event
	auto result = publish_sync(std::move(evt));
	//check event
	EXPECT_TRUE(static_cast<bool>(result->payload["success"]));

	auto duration = std::chrono::milliseconds(1200);
	std::this_thread::sleep_for(duration);

	auto evt2 = createEvent("session::check");
	evt2->payload["id"] = sessionID;
	auto result2 = publish_sync(std::move(evt2));
	EXPECT_TRUE(static_cast<bool>(result2->payload["success"]));
}

TEST_F(SessionManagerComponentTest, updateAndCheck) {

	//create event
	auto evt = createEvent("session::update");
	evt->sessionID = sessionID;
	evt->payload["seconds"] = 120;
	auto result = publish_sync(std::move(evt));
	EXPECT_TRUE(static_cast<bool>(result->payload["success"]));

	auto evt2 = createEvent("session::check");
	evt2->payload["id"] = sessionID;
	auto result2 = publish_sync(std::move(evt2));
	EXPECT_TRUE(static_cast<bool>(result2->payload["success"]));

	// seconds 0
	auto evt3 = createEvent("session::update");
	evt3->sessionID = sessionID;
	evt3->payload["seconds"] = 0;
	auto result3 = publish_sync(std::move(evt3));
	EXPECT_TRUE(static_cast<bool>(result3->payload["success"]));

	// seconds missing seconds
	auto evt4 = createEvent("session::update");
	evt4->sessionID = sessionID;
	auto result4 = publish_sync(std::move(evt4));
	EXPECT_TRUE(static_cast<bool>(result3->payload["success"]));

	// missing sessionid
	auto evt5 = createEvent("session::check");
	auto result5 = publish_sync(std::move(evt5));
	EXPECT_FALSE(static_cast<bool>(result5->payload["success"]));
	EXPECT_TRUE(hasErrorHeader(result5));
}


TEST_F(SessionManagerComponentTest, setAndgetAttribute) {

	//create event
	auto evt = createEvent("session::setAttribute");
	evt->payload["id"]    = sessionID;
	evt->payload["key"]   = "real";
	evt->payload["value"] = 200;
	//fire event
	auto result = publish_sync(std::move(evt));
	//check event
	EXPECT_TRUE(static_cast<bool>(result->payload["success"]));

	auto evt2 = createEvent("session::getAttribute");
	evt2->payload["id"]  = sessionID;
	evt2->payload["key"] = "real";
	auto result2 = publish_sync(std::move(evt2));
	EXPECT_EQ(200, static_cast<int>(result2->payload["value"]));
}

TEST_F(SessionManagerComponentTest, setAndgetAttributeWithMissingPayload) {

	//create event
	auto evt = createEvent("session::setAttribute");	
	evt->payload["key"]   = "real";
	evt->payload["value"] = 200;
	//fire event
	auto result = publish_sync(std::move(evt));
	//check event
	EXPECT_TRUE(hasErrorHeader(result));

	auto evt2 = createEvent("session::getAttribute");
	evt2->payload["key"] = "real";
	auto result2 = publish_sync(std::move(evt2));
	EXPECT_TRUE(hasErrorHeader(result2));
}

TEST_F(SessionManagerComponentTest, pushAttribute) {

	//create event
	auto evt = createEvent("session::pushAttribute");
	evt->payload["id"]    = sessionID;
	evt->payload["key"]   = "real";
	evt->payload["value"] = 200;
	//fire event
	auto result = publish_sync(std::move(evt));
	//check event
	EXPECT_TRUE(static_cast<bool>(result->payload["success"]));

	auto evt2 = createEvent("session::getAttribute");
	evt2->payload["id"]  = sessionID;
	evt2->payload["key"] = "real";
	auto result2 = publish_sync(std::move(evt2));
	EXPECT_EQ(200, static_cast<int>(result2->payload["value"]));

}

TEST_F(SessionManagerComponentTest, pushAttributeWithMissingPayload) {

	//create event
	auto evt = createEvent("session::pushAttribute");
	evt->payload["key"]   = "real";
	evt->payload["value"] = 200;
	//fire event
	auto result = publish_sync(std::move(evt));
	//check event
	EXPECT_TRUE(hasErrorHeader(result));
}

TEST_F(SessionManagerComponentTest, removeAttribute) {

	//create event
	auto evt = createEvent("session::removeAttribute");
	evt->payload["id"]    = sessionID;
	evt->payload["key"]   = "real";
	//fire event
	auto result = publish_sync(std::move(evt));
	//check event
	EXPECT_FALSE(static_cast<bool>(result->payload["success"]));

	auto evt2 = createEvent("session::setAttribute");
	evt2->payload["id"]  = sessionID;
	evt2->payload["key"]   = "real";
	evt2->payload["value"] = 200;
	auto result2 = publish_sync(std::move(evt2));
	EXPECT_TRUE(static_cast<bool>(result2->payload["success"]));

	auto evt3 = createEvent("session::removeAttribute");
	evt3->payload["id"]    = sessionID;
	evt3->payload["key"]   = "real";
	auto result3 = publish_sync(std::move(evt3));
	EXPECT_TRUE(static_cast<bool>(result3->payload["success"]));

	auto evt4 = createEvent("session::getAttribute");
	evt4->payload["id"]  = sessionID;
	evt4->payload["key"] = "real";
	auto result4 = publish_sync(std::move(evt4));
	EXPECT_TRUE(result4->payload["value"].isNull());
}

TEST_F(SessionManagerComponentTest, removeAttributeWithMissingPayload) {

	auto evt2 = createEvent("session::setAttribute");
	evt2->payload["id"]  = sessionID;
	evt2->payload["key"]   = "real";
	evt2->payload["value"] = 200;
	auto result2 = publish_sync(std::move(evt2));
	EXPECT_TRUE(static_cast<bool>(result2->payload["success"]));

	auto evt3 = createEvent("session::removeAttribute");	
	evt3->payload["key"]   = "real";
	auto result3 = publish_sync(std::move(evt3));
	//check event
	EXPECT_TRUE(hasErrorHeader(result3));
}