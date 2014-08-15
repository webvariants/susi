#include "gtest/gtest.h"
#include "iocontroller/IOController.h"
#include "rbac/RBACManager.h"

class RBACManagerTest : public ::testing::Test {
	public:
		RBACManagerTest(){};
		virtual ~RBACManagerTest(){}
	protected:
		Susi::RBACManager manager;
		Susi::IOController controller;
		virtual void SetUp() override {}
		virtual void TearDown() override {}
};

TEST_F(RBACManagerTest,LoadNonExistent){
	//EXPECT_FALSE(manager.loadFile("./test.json"));
}

TEST_F(RBACManagerTest,Defaults){
	manager.setDefaultRead(false);
	manager.setDefaultWrite(false);
	EXPECT_FALSE(manager.getRead("foo","role1"));
	EXPECT_FALSE(manager.getWrite("foo","role1"));
	manager.setRead("foo","role1",true);
	manager.setWrite("foo","role1",true);
	EXPECT_TRUE(manager.getRead("foo","role1"));
	EXPECT_TRUE(manager.getWrite("foo","role1"));
}