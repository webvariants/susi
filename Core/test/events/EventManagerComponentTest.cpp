#include "susi/util/ComponentTest.h"
#include <iostream>


// This component should be tested implicitly with testing all the other components
// The only Test Case not tested that way is tested below
class EventManagerComponentTest : public ComponentTest {
protected:

	virtual void SetUp() override {
		componentManager->startComponent("eventsystem");
	}

	virtual void TearDown() override {
		componentManager->stopComponent("eventsystem");
	}

	virtual void GoodCases() override {
	// Test Case: Publish after EventManagerComponent was stopped (Should fail)
		// Make sure publishing before works
		// Use a minimal consumer that sets the bool execute to true when it's called
		bool executed = false;
 		Susi::Events::Consumer c = [&executed](Susi::Events::SharedEventPtr event) {
 			executed = true;
 		};
 		subscribe(std::string{"eventmanagertest::stoptest"}, c);
 		publish_sync(std::move(createEvent("eventmanagertest::stoptest")));
 		EXPECT_TRUE(executed);

 		// Now test if a publish will work after the stop call
 		executed = false;
 		componentManager->stopComponent("eventsystem");
 		publish_sync(std::move(createEvent("eventmanagertest::stoptest")));
 		EXPECT_FALSE(executed);
	}

	virtual void BadCases() override {}

	virtual void EdgeCases() override {}

};

TEST_F(EventManagerComponentTest,GoodCases){
	GoodCases();
}
