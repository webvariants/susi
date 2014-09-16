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
