/*#include "gtest/gtest.h"
#include "world/World.h"
#include "Poco/Net/HTTPClientSession.h"
#include <Poco/URI.h>

#include <thread>
#include <chrono>

class WebstackAssetsTest : public ::testing::Test {
protected:
	virtual void SetUp() override {
		"";
		Susi::IOController io;
		io.makeDir("./assets/");
		io.writeFile("./assets/test.txt","foobar");
		world.setupEventSystem();
		world.setupHeartBeat();
		world.setupSessionManager();
		world.setupDBManager();
		world.setupLogger();
		world.setupHttpServer();
	}
	virtual void TearDown() override {
		Susi::IOController io;
		io.deletePath("./assets/");
	}
};

TEST_F(WebstackAssetsTest, SimpleGet) {
	Poco::URI uri("http://[::1]:8080/assets/test.txt");
	Poco::Net::HTTPClientSession session(uri.getHost(),uri.getPort());

	 // send request
    Poco::Net::HTTPRequest req(Poco::Net::HTTPRequest::HTTP_GET, uri.getPath(), Poco::Net::HTTPMessage::HTTP_1_1);
    session.sendRequest(req);

    // get response
    Poco::Net::HTTPResponse res;
	std::istream &is = session.receiveResponse(res);
	std::stringstream ss;
	std::copy(std::istream_iterator<char>(is),
		std::istream_iterator<char>(),
		std::ostream_iterator<char>(ss));
    
    std::cout << res.getStatus() << " " << res.getReason() << std::endl;
    EXPECT_EQ(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK,res.getStatus());
	EXPECT_EQ("foobar",ss.str());
}*/