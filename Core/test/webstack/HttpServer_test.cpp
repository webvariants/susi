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
	int status;
	virtual void SetUp() override {
		io.makeDir(base_path+"/assets");
		io.writeFile(base_path+"/assets/test.txt","foobar");
		io.writeFile(base_path+"/assets/test.svg", "<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
		componentManager->startComponent("httpserver");
		std::this_thread::sleep_for(std::chrono::milliseconds{150});
	}
	virtual void TearDown() override {
		componentManager->stopAll();
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
	int status = client.getStatus();

	//std::cout<<"Status: "<<status<<std::endl;
	//std::cout<<"Body: "<<body<<std::endl;

	EXPECT_EQ(200,status);
	EXPECT_EQ("foobar",body);
}

TEST_F(HttpServerTest, GetSVGAsset) {

	Susi::HttpClient client("http://[::1]:8080");

	auto body = client.get("/assets/test.svg");
	int status = client.getStatus();

	//std::cout<<"Status: "<<status<<std::endl;
	//std::cout<<"Body: "<<body<<std::endl;

	EXPECT_EQ(200,status);
	EXPECT_EQ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>",body);
}

TEST_F(HttpServerTest, GetMissingAsset) {

	Susi::HttpClient client("http://[::1]:8080");

	auto body = client.get("/assets/wrong_test.txt");
	status = client.getStatus();

	//std::cout<<"Status: "<<status<<std::endl;
	//std::cout<<"Body: "<<body<<std::endl;	

	EXPECT_EQ(404,status);
}

/*TEST_F(HttpServerTest, NotFound) {

	Susi::HttpClient client("http://[::1]:8080");

	auto body = client.get("/not/found");
	status = client.getStatus();

	//std::cout<<"Status: "<<status<<std::endl;
	//std::cout<<"Body: "<<body<<std::endl;	

	EXPECT_EQ(404,status);
}*/

TEST_F(HttpServerTest, PostToForm) {

	Susi::HttpClient client("http://[::1]:8080");
	client.addHeader("Content-Type","multipart/form-data; boundary=----WebKitFormBoundaryePkpFF7tjBAqx29L");
	std::string postData =  "------WebKitFormBoundaryePkpFF7tjBAqx29L\n"
							"Content-Disposition: form-data; name=\"uploadedfile\"; filename=\"hello.txt\"\n"
							"Content-Type: application/x-object\n"
							"\n"
							"foobar\n"
							"------WebKitFormBoundaryePkpFF7tjBAqx29L--\n";
	std::cout<<postData<<std::endl;
	auto body = client.post("/form",postData);

	std::cout<<"Status: "<<client.getStatus()<<std::endl;
	std::cout<<"Body: "<<body<<std::endl;

	
}
