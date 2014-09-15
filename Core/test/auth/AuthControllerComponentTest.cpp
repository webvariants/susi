#include "util/ComponentTest.h"

class AuthControllerComponentTest : public ComponentTest {

public:
	//on live system, sessionID will be set by handlePublish from ApiServerForComponent
	std::string sessionID = "abc";
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
		evt->sessionID = sessionID;
		//fire event
		auto result = publish_sync(std::move(evt));
		//check event
		EXPECT_TRUE(static_cast<bool>(result->payload["success"]));

		auto evt2 = createEvent("auth::isLoggedIn");
		evt2->sessionID = sessionID;
		auto result2 = publish_sync(std::move(evt2));
		EXPECT_TRUE(static_cast<bool>(result2->payload["success"]));

		auto evt3 = createEvent("auth::getUsername");
		evt3->sessionID = sessionID;
		auto result3 = publish_sync(std::move(evt3));
		EXPECT_EQ("John", static_cast<std::string>(result3->payload["username"]));
				
		auto evt4 = createEvent("auth::logout");		
		evt4->sessionID = sessionID;
		auto result4 = publish_sync(std::move(evt4));

		auto evt5 = createEvent("auth::isLoggedIn");
		evt5->sessionID = sessionID;
		auto result5 = publish_sync(std::move(evt5));
		EXPECT_FALSE(static_cast<bool>(result5->payload["success"]));
		
	}

	virtual void BadCases() override {
		//create event
		auto evt = createEvent("auth::login");
		evt->payload["username"] = "foo";
		evt->payload["password"] = "bar";
		evt->sessionID = sessionID;
		//fire event
		auto result = publish_sync(std::move(evt));
		//check event
		EXPECT_EQ(false, static_cast<bool>(result->payload["success"]));
	}

	virtual void EdgeCases() override {
		// missing param
		auto evt = createEvent("auth::login");
		evt->payload["username"] = "foo";
		evt->sessionID = sessionID;				
		auto result = publish_sync(std::move(evt));		

		//std::cout<<"EDGE CASE:"<<result->toAny().toString()<<std::endl;
		EXPECT_FALSE(static_cast<bool>(result->payload["success"]));

		auto headers = result->getHeaders();
		bool error_found = false;

		for(size_t i=0; i<headers.size(); ++i) {
			if(headers[i].first == "error") {
				error_found = true;
				break;
			}			
		}
		
		EXPECT_TRUE(error_found);

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
