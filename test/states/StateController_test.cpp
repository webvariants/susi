#include "gtest/gtest.h"
#include "iocontroller/IOController.h"
#include "world/World.h"
#include "logger/Logger.h"
#include "states/StateController.h"

class StateControllerTest : public ::testing::Test {
protected:
	std::shared_ptr<Susi::States::StateController> controller{nullptr};
	virtual void SetUp() override {
		world.setupLogger();
		world.setupEventSystem();
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
	EXPECT_EQ("bar",controller->getState("foo").extract<std::string>());
	EXPECT_TRUE(controller->setPersistentState("foo","bar"));
	EXPECT_EQ("bar",controller->getPersistentState("foo").extract<std::string>());
}

TEST_F(StateControllerTest,Overwrite){
	EXPECT_TRUE(controller->setState("foo","bar"));
	EXPECT_TRUE(controller->setState("foo","baz"));
	EXPECT_EQ("baz",controller->getState("foo").extract<std::string>());
	EXPECT_TRUE(controller->setPersistentState("foo","bar"));
	EXPECT_TRUE(controller->setPersistentState("foo","baz"));
	EXPECT_EQ("baz",controller->getPersistentState("foo").extract<std::string>());
}

TEST_F(StateControllerTest,PersistentSaveLoad){
	EXPECT_TRUE(controller->setPersistentState("foo","bar"));
	controller->savePersistent();
	EXPECT_TRUE(controller->loadPersistent());
	EXPECT_EQ("bar",controller->getPersistentState("foo").extract<std::string>());	
}


