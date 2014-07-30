#include "gtest/gtest.h"
#include "events/EventSystem.h"
#include "client/SusiClient.h"
#include "apiserver/TCPServer.h"
#include <condition_variable>
#include "world/World.h"

class ClientTest : public ::testing::Test {
protected:
	std::shared_ptr<Susi::SusiClient> client;
	virtual void SetUp() override {
		world.setupEventSystem();
		world.setupHeartBeat();
		world.setupSessionManager();
		world.setupTCPServer();
		world.setupLogger();
		Susi::setLogLevel(0);
		client = std::shared_ptr<Susi::SusiClient>{new Susi::SusiClient("[::1]:4000")};
	}
	virtual void TearDown() override {}
};

TEST_F(ClientTest,Basic){
	bool callbackCalled = false;
	client->subscribe("foobar",[&callbackCalled](Susi::Event & event){
		callbackCalled = true;
	});
	client->publish("foobar",Poco::Dynamic::Var{},"");
	client->getEvent();
	EXPECT_TRUE(callbackCalled);
	client->close();
}

TEST_F(ClientTest,Unsubscribe){
	bool callbackOneCalled = false;
	bool callbackTwoCalled = false;
	client->subscribe("one",[&callbackOneCalled](Susi::Event & event){
		callbackOneCalled = true;
	});
	client->subscribe("two",[&callbackTwoCalled](Susi::Event & event){
		callbackTwoCalled = true;
	});
	client->publish("one",Poco::Dynamic::Var{},"");
	client->getEvent();
	EXPECT_TRUE(callbackOneCalled);
	EXPECT_FALSE(callbackTwoCalled);
	callbackOneCalled = false;
	callbackTwoCalled = false;
	client->unsubscribe("one");
	client->publish("one",Poco::Dynamic::Var{},"");
	client->publish("two",Poco::Dynamic::Var{},"");
	client->getEvent();
	EXPECT_FALSE(callbackOneCalled);
	EXPECT_TRUE(callbackTwoCalled);
	client->close();
}
