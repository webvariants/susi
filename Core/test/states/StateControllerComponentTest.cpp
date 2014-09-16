#include "util/ComponentTest.h"

class StateControllerComponentTest : public ComponentTest {
protected:
	
	virtual void SetUp() override {
		componentManager->startComponent("statecontroller");
	}
	
	virtual void TearDown() override {
		componentManager->stopComponent("statecontroller");
	}

	virtual void GoodCases() override {
		//create event
		auto evt = createEvent("state::setState");
		evt->payload["stateID"] = "foo";
		evt->payload["value"] = "bar";		
		//fire event
		auto result = publish_sync(std::move(evt));
		//check event
		EXPECT_TRUE(static_cast<bool>(result->payload["success"]));

		auto evt2 = createEvent("state::getState");
		evt2->payload["stateID"] = "foo";
		auto result2 = publish_sync(std::move(evt2));
		EXPECT_TRUE(static_cast<bool>(result2->payload["success"]));
		EXPECT_EQ("bar",static_cast<std::string>(result2->payload["value"]));

		auto evt3 = createEvent("state::setPersistentState");
		evt3->payload["stateID"] = "pfoo";
		evt3->payload["value"] = "pbar";		
		auto result3 = publish_sync(std::move(evt3));
		EXPECT_TRUE(static_cast<bool>(result3->payload["success"]));

		auto evt4 = createEvent("state::getPersistentState");
		evt4->payload["stateID"] = "pfoo";
		auto result4 = publish_sync(std::move(evt4));
		EXPECT_TRUE(static_cast<bool>(result4->payload["success"]));
		EXPECT_EQ("pbar",static_cast<std::string>(result4->payload["value"]));

		auto evt5 = createEvent("state::saveState");
		auto result5 = publish_sync(std::move(evt5));
		EXPECT_TRUE(static_cast<bool>(result5->payload["success"]));		
	}

	virtual void BadCases() override {

	}

	virtual void EdgeCases() override {

	}

};

TEST_F(StateControllerComponentTest,GoodCases){
	GoodCases();
}

TEST_F(StateControllerComponentTest,BadCases){
	BadCases();
}

TEST_F(StateControllerComponentTest,EdgeCases){
	EdgeCases();
}
