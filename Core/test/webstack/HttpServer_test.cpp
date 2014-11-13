#include "gtest/gtest.h"
#include "world/ComponentManager.h"
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
		io.makeDir(base_path+"/upload");
		io.writeFile(base_path+"/assets/test.txt","foobar");
		io.writeFile(base_path+"/assets/test.svg", "<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
		componentManager->startComponent("httpserver");
		std::this_thread::sleep_for(std::chrono::milliseconds{150});
	}
	virtual void TearDown() override {
		componentManager->stopAll();
	}

	virtual void GoodCases() override {}
	virtual void BadCases() override {}
	virtual void EdgeCases() override {}

};

TEST_F(HttpServerTest, GetAsset) {

	Susi::HttpClient client("http://[::1]:8080");

	auto body = client.get("/assets/test.txt");
	int status = client.getStatus();

	EXPECT_EQ(200,status);
	EXPECT_EQ("foobar",body);
}

TEST_F(HttpServerTest, GetSVGAsset) {

	Susi::HttpClient client("http://[::1]:8080");

	auto body = client.get("/assets/test.svg");
	int status = client.getStatus();

	EXPECT_EQ(200,status);
	EXPECT_EQ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>",body);
}

TEST_F(HttpServerTest, GetMissingAsset) {

	Susi::HttpClient client("http://[::1]:8080");

	auto body = client.get("/assets/wrong_test.txt");
	status = client.getStatus();

	EXPECT_EQ(404,status);
}

TEST_F(HttpServerTest, NotFound) {

	Susi::HttpClient client("http://[::1]:8080");

	client.get("/not/found");
	status = client.getStatus();

	EXPECT_EQ(404,status);	
}

TEST_F(HttpServerTest, Redirect) {

	Susi::HttpClient client("http://[::1]:8080");

	client.get("/");
	status = client.getStatus();

	EXPECT_EQ(302,status);	
}

TEST_F(HttpServerTest, Session) {

	std::vector<std::pair<std::string,std::string>> headers;
	int status;

	Susi::HttpClient client("http://[::1]:8080");
	client.addHeader("Cookie", "susisession=123");
	client.get("/assets/test.txt");
	status = client.getStatus();
	headers = client.getHeaders();

/*
	for(size_t i=0; i<headers.size(); ++i) {
		std::cout<<"HEADER "<<i<<":"<<headers[i].first<<" , "<<headers[i].second<<std::endl;
	}
*/
	
	EXPECT_EQ(200,status);
	EXPECT_EQ(2, countSetCookieInHeaders(headers));

	std::this_thread::sleep_for(std::chrono::milliseconds(300));

	// get new sessionID
	std::string new_session = getSessionIDFromHeaders(headers);
	client.addHeader("Cookie", "susisession="+new_session);
	client.get("/assets/test.txt");
	status = client.getStatus();
	headers = client.getHeaders();

/*
	for(size_t i=0; i<headers.size(); ++i) {
		std::cout<<"HEADER "<<i<<":"<<headers[i].first<<" , "<<headers[i].second<<std::endl;
	}
*/

	EXPECT_EQ(200,status);
	EXPECT_EQ(0, countSetCookieInHeaders(headers));

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	client.addHeader("Cookie", "susisession="+new_session);
	client.get("/assets/test.txt");
	status = client.getStatus();
	headers = client.getHeaders();

	/*
	for(size_t i=0; i<headers.size(); ++i) {
		std::cout<<"HEADER "<<i<<":"<<headers[i].first<<" , "<<headers[i].second<<std::endl;
	}
	*/

	EXPECT_EQ(200,status);	
	EXPECT_EQ(2, countSetCookieInHeaders(headers));
}

TEST_F(HttpServerTest, PostToForm) {

	Susi::HttpClient client("http://[::1]:8080");
	client.addHeader("Content-Type","multipart/form-data; boundary=----WebKitFormBoundaryePkpFF7tjBAqx29L");
	std::string postData =  "------WebKitFormBoundaryePkpFF7tjBAqx29L\n"
							"Content-Disposition: form-data; name=\"uploadedfile\"; filename=\"hello.txt\"\n"
							"Content-Type: application/x-object\n"
							"\n"
							"foobar\n"
							"------WebKitFormBoundaryePkpFF7tjBAqx29L--\n";
	auto body = client.post("/form",postData);
	std::string expectedBody = "<html><head><title>SUSI</title></head> <body><h2>Upload</h2>\n<p style=\"text-align: center;font-size: 48px;\"></br></br></br>Name: uploadedfile<br>\nFile Name: hello.txt<br>\nType: application/x-object<br>\nSize: 6<br>\n</p></body></html>";
	EXPECT_EQ(200,client.getStatus());
	EXPECT_EQ(expectedBody,body);
	
}

TEST_F(HttpServerTest, WebSocket) {
	Susi::HttpClient client("http://[::1]:8080");
	client.connectWebSocket();
	EXPECT_EQ(101,client.getStatus());
	Susi::Util::Any packet = Susi::Util::Any::Object{
		{"type","publish"},
		{"data",Susi::Util::Any::Object{
			{"topic","foobar"}
		}}
	};
	client.send(packet.toJSONString());
	Susi::Util::Any resp1 = Susi::Util::Any::fromJSONString(client.recv());
	Susi::Util::Any resp2 = Susi::Util::Any::fromJSONString(client.recv());
	if(resp1["type"] == "ack"){
		EXPECT_EQ(Susi::Util::Any{"foobar"},resp1["data"]["topic"]);
		EXPECT_EQ(Susi::Util::Any{false},resp2["error"]);
	}else{
		EXPECT_EQ(Susi::Util::Any{false},resp1["error"]);
		EXPECT_EQ(Susi::Util::Any{"status"},resp1["type"]);
		EXPECT_EQ(Susi::Util::Any{"foobar"},resp2["data"]["topic"]);
		EXPECT_EQ(Susi::Util::Any{"ack"},resp2["type"]);
	}
}
