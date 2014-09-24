#include "gtest/gtest.h"
#include "db/Database.h"
#include "iocontroller/IOController.h"

class DatabaseTest : public ::testing::Test {
public:
    std::string base_path = Poco::Path(Poco::Path::current()).toString() + "dbtest/";
protected:
    Susi::IOController io;
    std::shared_ptr<Susi::DB::Database> db{nullptr};
    virtual void SetUp() override {
        io.makeDir(base_path);
    }
    virtual void TearDown() override {
        io.deletePath(base_path);
    }
};

TEST_F(DatabaseTest, Contruct) {
	EXPECT_NO_THROW({
		db = std::make_shared<Susi::DB::Database>("sqlite3", base_path + "test_sqlite_db");
	});
}

TEST_F(DatabaseTest, QuerySqlite) {
    db = std::make_shared<Susi::DB::Database>("sqlite3", base_path + "test_sqlite_db");

    Susi::Util::Any table_exits = db->query("select count(type) from sqlite_master where type=\'table\' and name=\'test1\';");
    std::string result_str = table_exits[0]["count(type)"];

    if(result_str == "0") {    
	   db->query("create table test1 ("
        "    id integer,"
        "    name varchar(100)"
        ");");
    }

    db->query("insert into test1(id, name) values(7, \'John\');");
    Susi::Util::Any result = db->query("select id, name from test1 where id = 7");

    EXPECT_TRUE(result.isArray());
    EXPECT_FALSE(result[0].isNull());
    
    auto idVar = result[0]["id"];
    auto nameVar = result[0]["name"];
    EXPECT_FALSE(idVar.isNull());
    EXPECT_FALSE(nameVar.isNull());
    EXPECT_TRUE(idVar.isInteger());
    EXPECT_TRUE(nameVar.isString());
    
    
    EXPECT_EQ(7,static_cast<int>(idVar));
    EXPECT_EQ("John",static_cast<std::string>(nameVar));
}