#include "gtest/gtest.h"
#include "db/Database.h"
#include "iocontroller/IOController.h"

class DatabaseTest : public ::testing::Test {
protected:
    Susi::IOController io;
    std::shared_ptr<Susi::DB::Database> db{nullptr};
    virtual void SetUp() override {
        io.makeDir("./dbtest/");
    }
    virtual void TearDown() override {
        io.deletePath("./dbtest/");
    }
};

TEST_F(DatabaseTest, Contruct) {
	EXPECT_NO_THROW({
		db = std::make_shared<Susi::DB::Database>("sqlite3", "./dbtest/test_sqlite_db");
	});
}

TEST_F(DatabaseTest, QuerySqlite) {
    db = std::make_shared<Susi::DB::Database>("sqlite3", "./dbtest/test_sqlite_db");

	Poco::Dynamic::Var table_exits = db->query("select count(type) from sqlite_master where type=\'table\' and name=\'test1\';");

    if(table_exits[0]["count(type)"].convert<int>() == 0) {
	   db->query("create table test1 ("
        "    id integer,"
        "    name varchar(100)"
        ");");
    }

    db->query("insert into test1(id, name) values(7, \'John\');");

    Poco::Dynamic::Var result = db->query("select id, name from test1 where id = 7");

    EXPECT_TRUE(result.isVector());

    EXPECT_FALSE(result[0].isEmpty());
    auto idVar = result[0]["id"];
    auto nameVar = result[0]["name"];
    EXPECT_FALSE(idVar.isEmpty());
    EXPECT_FALSE(nameVar.isEmpty());
    EXPECT_TRUE(idVar.isInteger());
    EXPECT_TRUE(nameVar.isString());
    EXPECT_EQ(7,idVar.convert<int>());
    EXPECT_EQ("John",nameVar.convert<std::string>());

    Susi::IOController controller;
    controller.deletePath("./test_sqlite_db");	
}