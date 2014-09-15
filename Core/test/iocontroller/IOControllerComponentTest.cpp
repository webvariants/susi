#include "util/ComponentTest.h"

class IOControllerComponentTest : public ComponentTest {
protected:
	
	virtual void SetUp() override {
		componentManager->startComponent("iocontroller");
	}
	
	virtual void TearDown() override {
		componentManager->stopComponent("iocontroller");
	}

	virtual void GoodCases() override {

	}

	virtual void BadCases() override {

	}

	virtual void EdgeCases() override {

	}

};

TEST_F(IOControllerComponentTest,GoodCases){
	GoodCases();
}

TEST_F(IOControllerComponentTest,BadCases){
	BadCases();
}

TEST_F(IOControllerComponentTest,EdgeCases){
	EdgeCases();
}
