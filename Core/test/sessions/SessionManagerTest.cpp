#include "util/ComponentTest.h"

class SessionManagerTest : public ComponentTest {
protected:
	std::shared_ptr<Susi::Sessions::SessionManager> sessionManager;
	std::string sessionID{"s1"};
	std::string sessionID_2{"s2"};

	virtual void SetUp() override {
		componentManager->startComponent("sessionmanager");

		sessionManager = componentManager->getComponent<Susi::Sessions::SessionManager>("sessionmanager");
	}
	
	virtual void TearDown() override {
		componentManager->stopComponent("sessionmanager");
	}

	virtual void GoodCases() override {}
	virtual void BadCases() override {}
	virtual void EdgeCases() override {}

};


TEST_F(SessionManagerTest, Init) {
	EXPECT_FALSE(sessionManager->checkSession(sessionID));	

	
	sessionManager->updateSession(sessionID);
	EXPECT_TRUE(sessionManager->checkSession(sessionID));

	auto duration = std::chrono::milliseconds(1100);
	std::this_thread::sleep_for(duration);
	EXPECT_FALSE(sessionManager->checkSession(sessionID));	
}

TEST_F(SessionManagerTest, CheckSessions) {
	
	sessionManager->init(std::chrono::milliseconds{100});
	sessionManager->updateSession(sessionID);
	sessionManager->updateSession(sessionID_2);
	EXPECT_TRUE(sessionManager->checkSession(sessionID));
	EXPECT_TRUE(sessionManager->checkSession(sessionID_2));

	auto duration = std::chrono::milliseconds(200);
	std::this_thread::sleep_for(duration);
	// expect 2 dead sessions
	EXPECT_EQ(2,sessionManager->checkSessions());	
}

TEST_F(SessionManagerTest, SetPushAttributeWithEmptySessionID){
	std::string sessionID_3{""};

	EXPECT_FALSE(sessionManager->setSessionAttribute(sessionID_3, "real", "superb"));
	EXPECT_FALSE(sessionManager->pushSessionAttribute(sessionID_3, "real", "superb"));
}

TEST_F(SessionManagerTest, UpdateExistingSessionWithDuration){
	std::string sessionID_4{"s4"};

	sessionManager->init(std::chrono::milliseconds{100});
	// will create session
	sessionManager->updateSession(sessionID_4);
	auto duration  = std::chrono::milliseconds(50);
	auto duration2 = std::chrono::milliseconds(200);
	
	// 50 from 100
	std::this_thread::sleep_for(duration);
	EXPECT_TRUE(sessionManager->checkSession(sessionID_4));

	// add 200
	sessionManager->updateSession(sessionID_4, duration2);

	// wait 200 more
	std::this_thread::sleep_for(duration2);
	// still alive, 300 not over
	EXPECT_TRUE(sessionManager->checkSession(sessionID_4));

	
	// wait 50 more , sum is now 300 
	std::this_thread::sleep_for(duration);
	// still alive ?
	EXPECT_FALSE(sessionManager->checkSession(sessionID_4));
}

TEST_F(SessionManagerTest, KillNonExistingSession){	
	std::string sessionID_5{"s5"};

	EXPECT_FALSE(sessionManager->killSession(sessionID_5));
}


TEST_F(SessionManagerTest, SetGetAttribute){
	sessionManager->updateSession(sessionID, std::chrono::milliseconds(20000));
	EXPECT_TRUE(sessionManager->setSessionAttribute(sessionID, "real", "superb"));
	auto res1 = sessionManager->getSessionAttribute(sessionID, "real");
	EXPECT_FALSE(res1.isNull());
	if(!res1.isNull()) {
		std::string str = res1;
		EXPECT_EQ("superb", str);
	}
}

TEST_F(SessionManagerTest, GetNonExistentAttribute){
	sessionManager->updateSession(sessionID, std::chrono::milliseconds(20000));
	auto res1 = sessionManager->getSessionAttribute(sessionID, "real");
	EXPECT_TRUE(res1.isNull());
}

TEST_F(SessionManagerTest, GetAttributeFromNonExistentSession){
	auto res1 = sessionManager->getSessionAttribute(sessionID, "real");
	EXPECT_TRUE(res1.isNull());
}

TEST_F(SessionManagerTest, PushGetAttribute){
	sessionManager->updateSession(sessionID, std::chrono::milliseconds(20000));
	EXPECT_TRUE(sessionManager->pushSessionAttribute(sessionID, "real", "superb"));
	auto res2 = sessionManager->getSessionAttribute(sessionID, "real");
	EXPECT_FALSE(res2.isNull());
	if(!res2.isNull()) {
		EXPECT_NO_THROW({
			std::string str = res2;
			EXPECT_EQ("superb", str);
		});
	}
}

TEST_F(SessionManagerTest, SetPushGetAttribute){
	sessionManager->updateSession(sessionID, std::chrono::milliseconds(20000));
	EXPECT_TRUE(sessionManager->setSessionAttribute(sessionID, "real", "superb_0"));
	EXPECT_TRUE(sessionManager->pushSessionAttribute(sessionID, "real", "superb_1"));
	auto res2 = sessionManager->getSessionAttribute(sessionID, "real");
	EXPECT_FALSE(res2.isNull());
	if(!res2.isNull()) {
		EXPECT_NO_THROW({
			std::string str0 = sessionManager->getSessionAttribute(sessionID, "real")[0];
			std::string str1 = sessionManager->getSessionAttribute(sessionID, "real")[1];

			EXPECT_EQ("superb_0", str0);
			EXPECT_EQ("superb_1", str1);
		});
	}
}