#include "util/ComponentTest.h"

class EngineStarterTest : public ComponentTest {
protected:
	std::shared_ptr<Susi::EngineStarter::Starter> engineStarter;
	Susi::IOController io;
	std::string script1;
	std::string script2;
	std::string output1;
	std::string output2;

	virtual void SetUp() override {
		script1 = base_path+"test.sh";
		output1 = base_path+"test.out";

		io.writeFile(script1,"#!/bin/bash\nfor i in $(seq 1 10); do echo -n foobar >> "+output1+"\nsleep 0.1; done; exit 0\n");
		io.setExecutable(script1,true);

		io.makeDir(base_path+"sub/");

		script2 = base_path+"sub/test.sh";
		output2 = base_path+"sub/test.out";		

		io.writeFile(script2,"#!/bin/bash\nfor i in $(seq 1 10); do echo -n foobar >> "+output2+"\nsleep 0.1; done; exit 0\n");
		io.setExecutable(script2,true);

		componentManager->startComponent("enginestarter");
		engineStarter = componentManager->getComponent<Susi::EngineStarter::Starter>("enginestarter");
	}
	
	virtual void TearDown() override {
		componentManager->stopComponent("enginestarter");
	}

	virtual void GoodCases() override {
		std::string data;

		// test also multi execution with sub
		engineStarter->execute(base_path);
		std::this_thread::sleep_for(std::chrono::milliseconds(1100));
		data = io.readFile(output1);
		EXPECT_EQ("foobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobar",data);

		data = io.readFile(output2);
		EXPECT_EQ("foobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobar",data);

		io.deletePath(output1);

		engineStarter->execute(base_path);
		std::this_thread::sleep_for(std::chrono::milliseconds(300));
		engineStarter->killall();		
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		data = io.readFile(output1);
		EXPECT_NE("foobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobar",data);		
	}

	virtual void BadCases() override {

	}

	virtual void EdgeCases() override {

	}

};

TEST_F(EngineStarterTest,GoodCases){
	GoodCases();
}

/*
TEST_F(EngineStarterTest,BadCases){
	BadCases();
}

TEST_F(EngineStarterTest,EdgeCases){
	EdgeCases();
}*/
