#include "gtest/gtest.h"
#include "susi/iocontroller/IOController.h"
#include "susi/logger/easylogging++.h"
#include "susi/states/StateController.h"

using Susi::Util::Any;

class StateControllerTest : public ::testing::Test {
public:
   	std::string base_path = Poco::Path(Poco::Path::current()).toString() + "state_test/";
   	Susi::IOController io;
protected:
	std::shared_ptr<Susi::States::StateController> controller{nullptr};
	virtual void SetUp() override {		
		io.makeDir(base_path);
		controller = std::make_shared<Susi::States::StateController>(std::string(base_path + "states.json"));
	}
	virtual void TearDown() override {
		io.deletePath(base_path);
	}
};

TEST_F(StateControllerTest,Simple){
	EXPECT_TRUE(controller->setState("foo","bar"));
	EXPECT_EQ(Any{"bar"},controller->getState("foo"));
	EXPECT_TRUE(controller->setPersistentState("foo","bar"));
	EXPECT_EQ(Any{"bar"},controller->getPersistentState("foo"));
}


TEST_F(StateControllerTest,Overwrite){
	EXPECT_TRUE(controller->setState("foo","bar"));
	EXPECT_TRUE(controller->setState("foo","baz"));
	EXPECT_EQ(Any{"baz"},controller->getState("foo"));
	
	EXPECT_TRUE(controller->setPersistentState("foo","bar"));
	EXPECT_TRUE(controller->setPersistentState("foo","baz"));
	EXPECT_EQ(Any{"baz"},controller->getPersistentState("foo"));
	
}

TEST_F(StateControllerTest,PersistentSaveLoad){
	EXPECT_TRUE(controller->setPersistentState("foo","bar"));
	controller->savePersistent();
	EXPECT_TRUE(controller->loadPersistent());
	EXPECT_EQ(Any{"bar"},controller->getPersistentState("foo"));	
}


