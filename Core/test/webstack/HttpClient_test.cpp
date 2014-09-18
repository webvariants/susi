#include "gtest/gtest.h"
#include "world/ComponentManager.h"
#include "world/system_setup.h"
#include "config/Config.h"
#include "iocontroller/IOController.h"

#include <thread>
#include <chrono>

#include "webstack/HttpClient.h"
#include "util/ComponentTest.h"

class HttpServerTest : public ComponentTest {
protected:
	Susi::IOController io;
	virtual void SetUp() override {
		io.makeDir("/tmp/assets");
		io.writeFile("/tmp/assets/test.txt","foobar");
		componentManager->startComponent("httpserver");
		std::this_thread::sleep_for(std::chrono::milliseconds{150});
	}
	virtual void TearDown() override {
		componentManager->stopAll();
		io.deletePath("/tmp/assets");
	}

	virtual void GoodCases() override {

	}
	virtual void BadCases() override {
		
	}
	virtual void EdgeCases() override {
		
	}

};

TEST_F(HttpServerTest, GetAsset) {

	Susi::HttpClient client("http://[::1]:8080");

	auto body = client.get("/assets/test.txt");

	std::cout<<"Status: "<<client.getStatus()<<std::endl;
	std::cout<<"Body: "<<body<<std::endl;

	EXPECT_EQ("foobar",body);
}

TEST_F(HttpServerTest, PostToForm) {

	Susi::HttpClient client("http://[::1]:8080");

	auto body = client.post("/form","var1=value1&var2=value2");

	std::cout<<"Status: "<<client.getStatus()<<std::endl;
	std::cout<<"Body: "<<body<<std::endl;

	
}
