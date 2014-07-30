#include <gtest/gtest.h>
#include <Poco/Dynamic/Var.h>
#include "sessions/SessionManager.h"
#include "world/World.h"

using namespace Susi::Sessions;

#include "world/World.h"

class SessionManagerTest : public ::testing::Test {
protected:
	Susi::Sessions::SessionManager sessionManager;
	void SetUp(){
		world.setupHeartBeat();
		sessionManager.init(std::chrono::milliseconds(250),std::chrono::milliseconds(250));
	}
};

TEST_F(SessionManagerTest, Init) {
	EXPECT_FALSE(sessionManager.checkSession("s1"));
	sessionManager.updateSession("s1");
	EXPECT_TRUE(sessionManager.checkSession("s1"));
	auto duration = std::chrono::milliseconds(250);
	std::this_thread::sleep_for(duration);
	EXPECT_FALSE(sessionManager.checkSession("s1"));
}

TEST_F(SessionManagerTest, SetGetAttribute){
	sessionManager.updateSession("s1", std::chrono::milliseconds(20000));
	EXPECT_TRUE(sessionManager.setSessionAttribute("s1", "real", "superb"));
	auto res1 = sessionManager.getSessionAttribute("s1", "real");
	EXPECT_FALSE(res1.isEmpty());
	if(!res1.isEmpty()) {
		EXPECT_EQ("superb", res1.extract<std::string>());
	}
}

TEST_F(SessionManagerTest, GetNonExistentAttribute){
	sessionManager.updateSession("s1", std::chrono::milliseconds(20000));
	auto res1 = sessionManager.getSessionAttribute("s1", "real");
	EXPECT_TRUE(res1.isEmpty());
}

TEST_F(SessionManagerTest, GetAttributeFromNonExistentSession){
	auto res1 = sessionManager.getSessionAttribute("s1", "real");
	EXPECT_TRUE(res1.isEmpty());
}

TEST_F(SessionManagerTest, PushGetAttribute){
	sessionManager.updateSession("s1", std::chrono::milliseconds(20000));
	EXPECT_TRUE(sessionManager.pushSessionAttribute("s1", "real", "superb"));
	auto res2 = sessionManager.getSessionAttribute("s1", "real");
	EXPECT_FALSE(res2.isEmpty());
	if(!res2.isEmpty()) {
		EXPECT_NO_THROW({
			EXPECT_EQ("superb", res2.convert<std::string>());
		});
	}
}

TEST_F(SessionManagerTest, SetPushGetAttribute){
	sessionManager.updateSession("s1", std::chrono::milliseconds(20000));
	EXPECT_TRUE(sessionManager.setSessionAttribute("s1", "real", "superb_0"));
	EXPECT_TRUE(sessionManager.pushSessionAttribute("s1", "real", "superb_1"));
	auto res2 = sessionManager.getSessionAttribute("s1", "real");
	EXPECT_FALSE(res2.isEmpty());
	if(!res2.isEmpty()) {
		EXPECT_NO_THROW({
			EXPECT_EQ("superb_0", sessionManager.getSessionAttribute("s1", "real")[0].convert<std::string>());
			EXPECT_EQ("superb_1", sessionManager.getSessionAttribute("s1", "real")[1].convert<std::string>());
		});
	}
}
