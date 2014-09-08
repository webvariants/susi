#include "gtest/gtest.h"
#include "auth/AuthController.h"
#include <condition_variable>

#include "world/World.h"
#include "logger/Logger.h"

class AuthControllerTest : public ::testing::Test {
protected:
	std::string sessionID = "ljcfbhsdlfsdpf434sdff";

	void SetUp() override {		
		world.setupEventManager();
		world.setupIOController();
		world.setupSessionManager();
		world.setupDBManager();
		world.setupAuthController();
		
		Susi::Logger::setLevel(Susi::Logger::NOTHING);

		auto db = world.dbManager->getDatabase("auth");
	
		db->query("create table users("
        "    id integer,"
        "    username varchar(100),"
        "    password varchar(100)"
        ");");

        db->query("insert into users(id, username, password) values(7, \'John\', \'Doe\');");	
	}

	virtual void TearDown() override {
		world.ioController->deletePath("./auth.db");
	}
};

TEST_F(AuthControllerTest, LogIn) {
	bool result = false;

	result = world.authController->login(sessionID, "Foo", "Bar");
	EXPECT_FALSE(result);

	result = world.authController->isLoggedIn(sessionID);
	EXPECT_FALSE(result);

	result = world.authController->login(sessionID, "John", "Doe");
	EXPECT_TRUE(result);

	result = world.authController->isLoggedIn(sessionID);
	EXPECT_TRUE(result);
}

TEST_F(AuthControllerTest, LogOut) {
	bool result = false;

	result = world.authController->login(sessionID, "John", "Doe");
	EXPECT_TRUE(result);

	result = world.authController->isLoggedIn(sessionID);
	EXPECT_TRUE(result);

	world.authController->logout(sessionID);

	result = world.authController->isLoggedIn(sessionID);
	EXPECT_FALSE(result);
}

TEST_F(AuthControllerTest, GetUserName) {
	std::string result = "foobar";

	result = world.authController->getUsername(sessionID);
	EXPECT_EQ("",result);

	bool login = world.authController->login(sessionID, "John", "Doe");
	EXPECT_TRUE(login);

	result = world.authController->getUsername(sessionID);
	EXPECT_EQ("John",result);
}