#include "gtest/gtest.h"
#include "db/Manager.h"
#include "util/VarHelper.h"
#include "iocontroller/IOController.h"

TEST(DBManager, Contruct) {

    Susi::DB::Manager dbm_1();
    //--------------------------------

    std::vector<std::tuple<std::string,std::string,std::string>> dbs;

    dbs.push_back(std::make_tuple("test_sqlite_db_1", "sqlite3", "./test_sqlite_db_1"));
    dbs.push_back(std::make_tuple("test_sqlite_db_2", "sqlite3", "./test_sqlite_db_2"));

    Susi::DB::Manager dbm_2(dbs);

    //-----------------------------
    Poco::Dynamic::Var config =  Poco::Dynamic::Var(dbs);

    Susi::DB::Manager dbm_3(config);    

    dbm_2.getDatabase("test_sqlite_db_1");
    dbm_3.getDatabase("test_sqlite_db_2");

    Susi::IOController controller;
    controller.deletePath("./test_sqlite_db_1");
    controller.deletePath("./test_sqlite_db_2");
}

TEST(DBManager, Sqlite3) {
	using Susi::Util::getFromVar;

	Susi::DB::Manager dm;

	// identifier, dbtype, connectURI
	//dm->addDatabase("test_firebird_db", "firebird", "service=/usr/local/firebird/db/test.fdb user=TEST password=TEST");
	dm.addDatabase("test_sqlite_db_1", "sqlite3", "./test_sqlite_db");

	// this creates the file if not exist
	auto db_1 = dm.getDatabase("test_sqlite_db_1");

    Poco::Dynamic::Var table_exits = db_1->query("select count(type) from sqlite_master where type=\'table\' and name=\'test1\';");

    //std::cout<<table_exits.toString()<<std::endl;

    if(table_exits[0]["count(type)"].convert<int>() == 0) {
	   db_1->query("create table test1 ("
        "    id integer,"
        "    name varchar(100)"
        ");");
    }

    db_1->query("insert into test1(id, name) values(7, \'John\');");


    auto db_2 = dm.getDatabase("test_sqlite_db_1");

    Poco::Dynamic::Var result = db_2->query("select id, name from test1 where id = 7");

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