#include "gtest/gtest.h"
#include "iocontroller/IOController.h"
#include "enginestarter/EngineStarter.h"

#include <chrono>
#include <thread>

class EngineStarterTest : public ::testing::Test {
protected:
	const std::string TMP_PATH = "./enginestarter_test/";
	Susi::EngineStarter enginestarter;
	Susi::IOController io;
	virtual void SetUp() override {
		io.makeDir(TMP_PATH);
	}
	virtual void TearDown() override {
		io.deletePath(TMP_PATH);
	}
public:
	EngineStarterTest() : enginestarter(TMP_PATH) {}
};

TEST_F(EngineStarterTest, ExecuteOne) {
	std::string script = TMP_PATH+"test.sh";
	std::string output = TMP_PATH+"test.out";
	io.writeFile(script,"#!/bin/bash\necho -n foobar > "+output+"\nexit 0\n");
	io.setExecutable(script,true);
	enginestarter.execute();
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	std::string data = io.readFile(output);
	EXPECT_EQ("foobar",data);
}

TEST_F(EngineStarterTest, KillAll) {
	std::string script = TMP_PATH+"test.sh";
	std::string output = TMP_PATH+"test.out";
	io.writeFile(script,"#!/bin/bash\nfor i in $(seq 1 100); do echo -n foobar >> "+output+"\nsleep 0.1; done; exit 0\n");
	io.setExecutable(script,true);
	enginestarter.execute();
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	enginestarter.killall();
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	std::string data = io.readFile(output);
	EXPECT_EQ("foobar",data);
}

TEST_F(EngineStarterTest, ExecuteMultiple) {
	std::string script1 = TMP_PATH+"test1.sh";
	std::string script2 = TMP_PATH+"test2.sh";
	std::string output1 = TMP_PATH+"test1.out";
	std::string output2 = TMP_PATH+"test2.out";
	io.writeFile(script1,"#!/bin/bash\necho -n foobar > "+output1+"\nexit 0\n");
	io.setExecutable(script1,true);
	io.writeFile(script2,"#!/bin/bash\necho -n foobar > "+output2+"\nexit 0\n");
	io.setExecutable(script2,true);
	enginestarter.execute();
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	std::string data1 = io.readFile(output1);
	EXPECT_EQ("foobar",data1);
	std::string data2 = io.readFile(output2);
	EXPECT_EQ("foobar",data2);
}

TEST_F(EngineStarterTest, ExecuteInSubdir) {
	std::string script = TMP_PATH+"subdir/"+"test.sh";
	std::string output = TMP_PATH+"test.out";
	io.makeDir(TMP_PATH+"subdir/");
	io.writeFile(script,"#!/bin/bash\necho -n foobar > "+output+"\nexit 0\n");
	io.setExecutable(script,true);
	enginestarter.execute();
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	std::string data = io.readFile(output);
	EXPECT_EQ("foobar",data);
}
