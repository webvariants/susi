#include "gtest/gtest.h"
#include "world/ComponentManager.h"
#include "world/system_setup.h"
#include "config/Config.h"
#include "iocontroller/IOController.h"

#include "webstack/HttpClient.h"

class HttpClientTest : public ::testing::Test {
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
		"			\"assets\": \"./clienttest\""
		"		}"
		"	}";

		componentManager = Susi::System::createSusiComponentManager(Susi::Util::Any::fromString(config));
		componentManager->startAll();

		io.makeDir(base_path + "test");
		io.writeFile(base_path + "test/test.txt","foobar");
	}
	virtual void TearDown() override {
		io.deletePath(base_path );
		componentManager->stopAll();
	}
};

TEST_F(HttpClientTest, Contruct) {

	Susi::HttpClient client("http://[::1]:8080/");
	
	std::string result = client.get(base_path + "assets/test/test.txt");

	EXPECT_EQ("foobar",result);

	//client.post("form");
}
