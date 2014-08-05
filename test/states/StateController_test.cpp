#include "gtest/gtest.h"
#include "iocontroller/IOController.h"
#include "world/World.h"
#include "logger/Logger.h"
#include "states/StateController.h"

using Susi::Util::Any;

class StateControllerTest : public ::testing::Test {
protected:
	std::shared_ptr<Susi::States::StateController> controller{nullptr};
	virtual void SetUp() override {
		world.setupLogger();
		world.setupEventManager();
		world.setupHeartBeat();
		world.setupIOController();
		controller = std::make_shared<Susi::States::StateController>(std::string("./states.json"));
	}
	virtual void TearDown() override {
		world.ioController->deletePath("./states.json");
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


