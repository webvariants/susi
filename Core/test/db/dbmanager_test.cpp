#include "gtest/gtest.h"
#include "db/Manager.h"
#include "iocontroller/IOController.h"


TEST(DBManager, Contruct) {

    Susi::DB::Manager dbm_1;
    //--------------------------------

    std::vector<std::tuple<std::string,std::string,std::string>> dbs;

    dbs.push_back(std::make_tuple("test_sqlite_db_1", "sqlite3", "./test_sqlite_db_1"));
    dbs.push_back(std::make_tuple("test_sqlite_db_2", "sqlite3", "./test_sqlite_db_2"));

    Susi::DB::Manager dbm_2(dbs);

    //-----------------------------    
    Susi::Util::Any config{Susi::Util::Any::Array{
        Susi::Util::Any::Object{{"identifier","test_sqlite_db_3"}, {"type","sqlite3"}, {"uri","./test_sqlite_db_3"}},
        Susi::Util::Any::Object{{"identifier","test_sqlite_db_4"}, {"type","sqlite3"}, {"uri","./test_sqlite_db_4"}}
    }};

    Susi::DB::Manager dbm_3(config);    

    EXPECT_NE(nullptr,dbm_2.getDatabase("test_sqlite_db_1"));
    EXPECT_NE(nullptr,dbm_3.getDatabase("test_sqlite_db_3"));

    Susi::IOController controller;
    controller.deletePath("./test_sqlite_db_1");
    controller.deletePath("./test_sqlite_db_3");
}



TEST(DBManager, Sqlite3) {
	Susi::DB::Manager dm;

	// identifier, dbtype, connectURI
	//dm->addDatabase("test_firebird_db", "firebird", "service=/usr/local/firebird/db/test.fdb user=TEST password=TEST");
	dm.addDatabase("test_sqlite_db_1", "sqlite3", "./test_sqlite_db");

	// this creates the file if not exist
	auto db_1 = dm.getDatabase("test_sqlite_db_1");

    Susi::Util::Any table_exits1 = db_1->query("select count(type) from sqlite_master where type=\'table\' and name=\'test1\';");
    std::string result_str1 = table_exits1[0]["count(type)"];

    if(result_str1 == "0") {    
	   db_1->query("create table test1 ("
        "    id integer,"
        "    name varchar(100)"
        ");");
    }

    db_1->query("insert into test1(id, name) values(7, \'John\');");


    auto db_2 = dm.getDatabase("test_sqlite_db_1");

    Susi::Util::Any result = db_2->query("select id, name from test1 where id = 7");

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

    Susi::IOController controller;
    controller.deletePath("./test_sqlite_db");
}