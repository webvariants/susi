/*
#include <gtest/gtest.h>
//#include <Poco/Dynamic/Var.h>
#include "sessions/SessionManager.h"
#include "world/World.h"

using namespace Susi::Sessions;

class SessionManagerTest : public ::testing::Test {
protected:
	std::string sessionID{"s1"};
	Susi::Sessions::SessionManager sessionManager;
	virtual void SetUp() override {
		std::cout<<"setup!"<<std::endl;
		world.setupEventManager();
		world.setupHeartBeat();
		sessionManager.init(std::chrono::milliseconds(250));
		std::cout<<"setup finished!"<<std::endl;
	}
	virtual void TearDown() override {
		std::cout<<"teardown"<<std::endl;
		world.tearDown();
		std::cout<<"teardown finished!"<<std::endl;
	}
};


// @FIXME when world object will be removed
TEST_F(SessionManagerTest, Init) {
	EXPECT_FALSE(sessionManager.checkSession(sessionID));
	sessionManager.updateSession(sessionID);
	EXPECT_TRUE(sessionManager.checkSession(sessionID));
	auto duration = std::chrono::milliseconds(350);
	std::this_thread::sleep_for(duration);
	EXPECT_FALSE(sessionManager.checkSession(sessionID));
}



TEST_F(SessionManagerTest, SetGetAttribute){
	sessionManager.updateSession(sessionID, std::chrono::milliseconds(20000));
	EXPECT_TRUE(sessionManager.setSessionAttribute(sessionID, "real", "superb"));
	auto res1 = sessionManager.getSessionAttribute(sessionID, "real");
	EXPECT_FALSE(res1.isNull());
	if(!res1.isNull()) {
		std::string str = res1;
		EXPECT_EQ("superb", str);
	}
}

TEST_F(SessionManagerTest, GetNonExistentAttribute){
	sessionManager.updateSession(sessionID, std::chrono::milliseconds(20000));
	auto res1 = sessionManager.getSessionAttribute(sessionID, "real");
	EXPECT_TRUE(res1.isNull());
}

TEST_F(SessionManagerTest, GetAttributeFromNonExistentSession){
	auto res1 = sessionManager.getSessionAttribute(sessionID, "real");
	EXPECT_TRUE(res1.isNull());
}

TEST_F(SessionManagerTest, PushGetAttribute){
	sessionManager.updateSession(sessionID, std::chrono::milliseconds(20000));
	EXPECT_TRUE(sessionManager.pushSessionAttribute(sessionID, "real", "superb"));
	auto res2 = sessionManager.getSessionAttribute(sessionID, "real");
	EXPECT_FALSE(res2.isNull());
	if(!res2.isNull()) {
		EXPECT_NO_THROW({
			std::string str = res2;
			EXPECT_EQ("superb", str);
		});
	}
}

TEST_F(SessionManagerTest, SetPushGetAttribute){
	sessionManager.updateSession(sessionID, std::chrono::milliseconds(20000));
	EXPECT_TRUE(sessionManager.setSessionAttribute(sessionID, "real", "superb_0"));
	EXPECT_TRUE(sessionManager.pushSessionAttribute(sessionID, "real", "superb_1"));
	auto res2 = sessionManager.getSessionAttribute(sessionID, "real");
	EXPECT_FALSE(res2.isNull());
	if(!res2.isNull()) {
		EXPECT_NO_THROW({
			std::string str0 = sessionManager.getSessionAttribute(sessionID, "real")[0];
			std::string str1 = sessionManager.getSessionAttribute(sessionID, "real")[1];

			EXPECT_EQ("superb_0", str0);
			EXPECT_EQ("superb_1", str1);
		});
	}
}

*/