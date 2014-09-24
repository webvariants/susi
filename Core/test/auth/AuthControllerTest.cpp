#include "util/ComponentTest.h"

class AuthControllerTest : public ComponentTest {
protected:
	std::shared_ptr<Susi::Auth::Controller> authController;
	std::string sessionID = "ljcfbhsdlfsdpf434sdff";
	virtual void SetUp() override {
		componentManager->startComponent("authcontroller");


		auto evt = createEvent("db::query");
		evt->payload["identifier"] = "auth";
		evt->payload["query"] = "create table users( id integer, username varchar(100), password varchar(100));";

		publish_sync(std::move(evt));

		auto evt2 = createEvent("db::query");
			evt2->payload["identifier"] = "auth";
			evt2->payload["query"] = "insert into users(id, username, password) values(7, \'John\', \'Doe\');";

		publish_sync(std::move(evt2));

		authController = componentManager->getComponent<Susi::Auth::Controller>("authcontroller");

	}
	
	virtual void TearDown() override {
		componentManager->stopComponent("authcontroller");
	}

	virtual void GoodCases() override {
		bool result = false;		
		std::string username = "";

		result = authController->login(sessionID, "Foo", "Bar");
		EXPECT_FALSE(result);

		result = authController->isLoggedIn(sessionID);
		EXPECT_FALSE(result);

		username = authController->getUsername(sessionID);
		EXPECT_EQ("",username);

		result = authController->login(sessionID, "John", "Doe");
		EXPECT_TRUE(result);

		result = authController->isLoggedIn(sessionID);
		EXPECT_TRUE(result);

		// check allready logged in
		result = authController->login(sessionID, "John", "Doe");
		EXPECT_TRUE(result);

		username = authController->getUsername(sessionID);
		EXPECT_EQ("John",username);
		
		authController->logout(sessionID);

		result = authController->isLoggedIn(sessionID);
		EXPECT_FALSE(result);
		
	}

	virtual void BadCases() override {

	}

	virtual void EdgeCases() override {

	}

};

TEST_F(AuthControllerTest,GoodCases){
	GoodCases();
}

TEST_F(AuthControllerTest,BadCases){
	BadCases();
}

TEST_F(AuthControllerTest,EdgeCases){
	EdgeCases();
}
