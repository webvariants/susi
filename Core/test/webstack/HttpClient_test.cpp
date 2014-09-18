#include "gtest/gtest.h"
#include "world/ComponentManager.h"
#include "world/system_setup.h"
#include "config/Config.h"
#include "iocontroller/IOController.h"

#include <thread>
#include <chrono>

#include "webstack/HttpClient.h"

class HttpServerTest : public ::testing::Test {
public:
   	std::string base_path = Poco::Path(Poco::Path::current()).toString() + "clienttest/";
protected:
	Susi::IOController io;
	std::shared_ptr<Susi::System::ComponentManager> componentManager;
	virtual void SetUp() override {

		std::string config = "{"
		"		\"eventsystem\" : {"
		"			\"threads\": 4,"
		"			\"queuelen\": 32"
		"		},"
		"		\"sessionmanager\": {"
		"			\"lifetime\": 600000,"
		"			\"checkInterval\": 1000"
		"		},"
		"		\"httpserver\": {"
		"			\"address\": \"[::1]:8080\","
		"			\"assets\": \""+base_path+"\""
		"		}"
		"	}";

		Susi::Logger::log("HttpClient Base Path: "+base_path);

		componentManager = Susi::System::createSusiComponentManager(Susi::Util::Any::fromString(config));
		componentManager->startAll();

		io.makeDir(base_path + "/test");
		io.writeFile(base_path + "/test/test.txt","foobar");
		std::this_thread::sleep_for(std::chrono::milliseconds{250});
	}
	virtual void TearDown() override {
		io.deletePath(base_path );
		componentManager->stopAll();
	}
};

TEST_F(HttpServerTest, GetAsset) {

	Susi::HttpClient client("http://[::1]:8080");

	auto result = client.get("/assets/test/test.txt");
	std::string body = result.second;

	std::cout<<"Status: "<<result.first->getStatus()<<std::endl;
	std::cout<<"Body: "<<result.second<<std::endl;

	EXPECT_EQ("foobar",body);
}

TEST_F(HttpServerTest, PostToForm) {

	Susi::HttpClient client("http://[::1]:8080");
	auto result = client.post("var1=value1&var2=value2", "/form");
	std::string body = result.second;

	std::cout<<"Status: "<<result.first->getStatus()<<std::endl;
	std::cout<<"Body: "<<result.second<<std::endl;

	EXPECT_EQ("var1=value1&var2=value2",body);
}
