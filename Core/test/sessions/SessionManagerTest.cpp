#include "util/ComponentTest.h"

class SessionManagerTest : public ComponentTest {
protected:
	std::shared_ptr<Susi::Sessions::SessionManager> sessionManager;
	std::string sessionID{"s1"};

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