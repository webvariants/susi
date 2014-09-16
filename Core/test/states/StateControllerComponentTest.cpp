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
