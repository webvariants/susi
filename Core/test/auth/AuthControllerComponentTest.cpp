#include "util/ComponentTest.h"

class AuthControllerComponentTest : public ComponentTest {
protected:
	
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


	}	
	
	virtual void TearDown() override {
		componentManager->stopComponent("authcontroller");
	}

	virtual void GoodCases() override {
		//create event
		auto evt = createEvent("auth::login");
		evt->payload["username"] = "John";
		evt->payload["password"] = "Doe";
		//fire event
		auto result = publish_sync(std::move(evt));
		//check event
		EXPECT_EQ(true, static_cast<bool>(result->payload["success"]));
	}

	virtual void BadCases() override {

	}

	virtual void EdgeCases() override {

	}

};

TEST_F(AuthControllerComponentTest,GoodCases){
	GoodCases();
}

TEST_F(AuthControllerComponentTest,BadCases){
	BadCases();
}

TEST_F(AuthControllerComponentTest,EdgeCases){
	EdgeCases();
}
