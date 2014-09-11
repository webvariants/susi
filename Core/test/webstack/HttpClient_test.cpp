#include "gtest/gtest.h"
#include "world/ComponentManager.h"
#include "world/system_setup.h"
#include "config/Config.h"
#include "iocontroller/IOController.h"

#include "webstack/HttpClient.h"

class HttpClientTest : public ::testing::Test {
protected:
	Susi::IOController io;
	virtual void SetUp() override {
		io.makeDir("./configtest/");
	}
	virtual void TearDown() override {
		io.deletePath("./configtest/");
	}
};

TEST_F(HttpClientTest, Contruct) {

	Susi::HttpClient client("http://[::1]:8080/");
	
	std::string result = client.get("assets/test/test.txt");

	EXPECT_EQ("foobar",result);

	//client.post("form");
}
