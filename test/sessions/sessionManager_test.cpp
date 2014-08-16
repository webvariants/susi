#include <gtest/gtest.h>
//#include <Poco/Dynamic/Var.h>
#include "sessions/SessionManager.h"
#include "world/World.h"

using namespace Susi::Sessions;

class SessionManagerTest : public ::testing::Test {
protected:
	virtual void SetUp() override {
		world.setupEventManager();
		//world.setupHeartBeat();
	}
	virtual void TearDown() override {
		world.tearDown();
	}
};


TEST_F(SessionManagerTest, Init) {
	Susi::Sessions::SessionManager sessionManager;
	sessionManager.init(std::chrono::milliseconds(250),std::chrono::milliseconds(250));
	EXPECT_FALSE(sessionManager.checkSession("s1"));
	sessionManager.updateSession("s1");
	EXPECT_TRUE(sessionManager.checkSession("s1"));
	auto duration = std::chrono::milliseconds(250);
	std::this_thread::sleep_for(duration);
	EXPECT_FALSE(sessionManager.checkSession("s1"));
}


TEST_F(SessionManagerTest, SetGetAttribute){
	Susi::Sessions::SessionManager sessionManager;
	sessionManager.init(std::chrono::milliseconds(250),std::chrono::milliseconds(250));
	sessionManager.updateSession("s1", std::chrono::milliseconds(20000));
	EXPECT_TRUE(sessionManager.setSessionAttribute("s1", "real", "superb"));
	auto res1 = sessionManager.getSessionAttribute("s1", "real");
	EXPECT_FALSE(res1.isNull());
	if(!res1.isNull()) {
		std::string str = res1;
		EXPECT_EQ("superb", str);
	}
}

TEST_F(SessionManagerTest, GetNonExistentAttribute){
	Susi::Sessions::SessionManager sessionManager;
	sessionManager.init(std::chrono::milliseconds(250),std::chrono::milliseconds(250));
	sessionManager.updateSession("s1", std::chrono::milliseconds(20000));
	auto res1 = sessionManager.getSessionAttribute("s1", "real");
	EXPECT_TRUE(res1.isNull());
}

TEST_F(SessionManagerTest, GetAttributeFromNonExistentSession){
	Susi::Sessions::SessionManager sessionManager;
	sessionManager.init(std::chrono::milliseconds(250),std::chrono::milliseconds(250));
	auto res1 = sessionManager.getSessionAttribute("s1", "real");
	EXPECT_TRUE(res1.isNull());
}

TEST_F(SessionManagerTest, PushGetAttribute){
	Susi::Sessions::SessionManager sessionManager;
	sessionManager.init(std::chrono::milliseconds(250),std::chrono::milliseconds(250));
	sessionManager.updateSession("s1", std::chrono::milliseconds(20000));
	EXPECT_TRUE(sessionManager.pushSessionAttribute("s1", "real", "superb"));
	auto res2 = sessionManager.getSessionAttribute("s1", "real");
	EXPECT_FALSE(res2.isNull());
	if(!res2.isNull()) {
		EXPECT_NO_THROW({
			std::string str = res2;
			EXPECT_EQ("superb", str);
		});
	}
}

TEST_F(SessionManagerTest, SetPushGetAttribute){
	Susi::Sessions::SessionManager sessionManager;
	sessionManager.init(std::chrono::milliseconds(250),std::chrono::milliseconds(250));
	sessionManager.updateSession("s1", std::chrono::milliseconds(20000));
	EXPECT_TRUE(sessionManager.setSessionAttribute("s1", "real", "superb_0"));
	EXPECT_TRUE(sessionManager.pushSessionAttribute("s1", "real", "superb_1"));
	auto res2 = sessionManager.getSessionAttribute("s1", "real");
	EXPECT_FALSE(res2.isNull());
	if(!res2.isNull()) {
		EXPECT_NO_THROW({
			std::string str0 = sessionManager.getSessionAttribute("s1", "real")[0];
			std::string str1 = sessionManager.getSessionAttribute("s1", "real")[1];

			EXPECT_EQ("superb_0", str0);
			EXPECT_EQ("superb_1", str1);
		});
	}
}
