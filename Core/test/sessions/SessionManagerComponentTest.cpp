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

TEST_F(SessionManagerComponentTest, updateAndCheck) {

	//create event
	auto evt = createEvent("session::update");
	evt->payload["id"] = sessionID;
	evt->payload["seconds"] = 120;
	//fire event
	auto result = publish_sync(std::move(evt));
	//check event
	EXPECT_TRUE(static_cast<bool>(result->payload["success"]));

	auto evt2 = createEvent("session::check");
	evt2->payload["id"] = sessionID;
	auto result2 = publish_sync(std::move(evt2));
	EXPECT_TRUE(static_cast<bool>(result2->payload["success"]));
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