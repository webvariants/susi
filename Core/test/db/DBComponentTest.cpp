#include "susi/util/ComponentTest.h"

class DBComponentTest : public ComponentTest {
protected:
	
	virtual void SetUp() override {
		componentManager->startComponent("dbmanager");

		auto evt = createEvent("db::query");
		evt->payload["identifier"] = "test_db";
		evt->payload["query"] = "create table users( id integer, username varchar(100), password varchar(100));";

		publish_sync(std::move(evt));

		auto evt2 = createEvent("db::query");
			evt2->payload["identifier"] = "test_db";
			evt2->payload["query"] = "insert into users(id, username, password) values(7, \'John\', \'Doe\');";

		publish_sync(std::move(evt2));
	}
	
	virtual void TearDown() override {
		componentManager->stopComponent("dbmanager");
	}

	virtual void GoodCases() override {
		//create event
		auto evt = createEvent("db::query");
		evt->payload["identifier"] = "test_db";
		evt->payload["query"] = "SELECT id FROM users WHERE username=\'John\' AND password=\'Doe\'";
		//fire event
		auto result = publish_sync(std::move(evt));
		//check event
		EXPECT_TRUE(static_cast<bool>(result->payload["success"]));

		//std::cout<<"EDGE CASE:"<<result->toAny().toJSONString()<<std::endl;

		Susi::Util::Any db_result = result->payload["result"];
		int id = db_result[0]["id"];

		EXPECT_EQ(7, id);

	}

	virtual void BadCases() override {
		
		// wrong identifier
		auto evt = createEvent("db::query");
		evt->payload["identifier"] = "db_dont_exists";
		evt->payload["query"] = "SELECT id FROM users WHERE username=\'John\' AND password=\'Doe\'";		
		auto result = publish_sync(std::move(evt));
		EXPECT_FALSE(static_cast<bool>(result->payload["success"]));
		EXPECT_TRUE(hasErrorHeader(result));

		//wrong query
		auto evt2 = createEvent("db::query");
		evt2->payload["identifier"] = "test_db";
		evt2->payload["query"] = "SELECT id FROM usersssss WHERE username=\'John\' AND password=\'Doe\'";		
		auto result2 = publish_sync(std::move(evt2));
		EXPECT_FALSE(static_cast<bool>(result2->payload["success"]));
		EXPECT_TRUE(hasErrorHeader(result2));

	}

	virtual void EdgeCases() override {
		//missing params
		auto evt = createEvent("db::query");		
		auto result = publish_sync(std::move(evt));
		EXPECT_FALSE(static_cast<bool>(result->payload["success"]));
		EXPECT_TRUE(hasErrorHeader(result));

	}

};

TEST_F(DBComponentTest,GoodCases){
	GoodCases();
}

TEST_F(DBComponentTest,BadCases){
	BadCases();
}

TEST_F(DBComponentTest,EdgeCases){
	EdgeCases();
}
