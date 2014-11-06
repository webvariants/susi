/*#include "gtest/gtest.h"
#include "events/EventSystem.h"
#include "client/SusiClient.h"
#include "apiserver/TCPServer.h"
#include <condition_variable>
#include "world/World.h"

class ApiServerTest : public ::testing::Test {
protected:
	virtual void SetUp() override {
		world.setupEventSystem();
		world.setupHeartBeat();
		world.setupSessionManager();
		world.setupTCPServer();
		world.setupLogger();
		"";
	}
	virtual void TearDown() override {}
};

TEST_F(ApiServerTest,Basic){
	Susi::SusiClient client(std::string("[::1]:4000"));
	bool callbackCalled = false;
	client.subscribe("test1",[&callbackCalled](Susi::Event & event){
		callbackCalled = true;
		EXPECT_EQ("test1",event.topic);
		EXPECT_TRUE(event.payload.isEmpty());
		EXPECT_EQ("",event.returnAddr);
	});
	client.publish("test1",Poco::Dynamic::Var{},"");
	client.getEvent();
	EXPECT_TRUE(callbackCalled);

	callbackCalled = false;
	client.subscribe("test2",[&callbackCalled](Susi::Event & event){
		callbackCalled = true;
		EXPECT_EQ("test2",event.topic);
		EXPECT_EQ("bar",Susi::Util::getFromVar(event.payload,"foo").convert<std::string>());
		EXPECT_EQ("blablub",event.returnAddr);
	});
	client.publish("test2",Susi::Event::Payload({{"foo","bar"}}),"blablub");
	client.getEvent();
	EXPECT_TRUE(callbackCalled);
	client.close();
}

TEST_F(ApiServerTest,Subscribe){
	Poco::Net::SocketAddress addr("[::1]:4000");
	Poco::Net::StreamSocket sock(addr);
	Susi::TCPConnection conn(sock);

	//First subscribe. Should be ok.
	conn.send(Susi::Event::Payload({
		{"type","subscribe"},
		{"key","foobar"}
	}));
	Poco::Dynamic::Var packet;
	bool ok = conn.recv(packet);
	EXPECT_TRUE(ok);
	auto typeVar = Susi::Util::getFromVar(packet,"type");
	auto keyVar = Susi::Util::getFromVar(packet,"key");
	auto payloadVar = Susi::Util::getFromVar(packet,"payload");
	EXPECT_FALSE(typeVar.isEmpty());
	EXPECT_FALSE(keyVar.isEmpty());
	EXPECT_FALSE(payloadVar.isEmpty());
	EXPECT_TRUE(typeVar.isString());
	EXPECT_TRUE(keyVar.isString());
	EXPECT_TRUE(payloadVar.isString());
	EXPECT_EQ("status",typeVar.convert<std::string>());
	EXPECT_EQ("ok",keyVar.convert<std::string>());
	EXPECT_EQ("successfuly subscribed to foobar",payloadVar.convert<std::string>());

	//second subscribe should fail
	conn.send(Susi::Event::Payload({
		{"type","subscribe"},
		{"key","foobar"}
	}));
	ok = conn.recv(packet);
	EXPECT_TRUE(ok);
	typeVar = Susi::Util::getFromVar(packet,"type");
	keyVar = Susi::Util::getFromVar(packet,"key");
	payloadVar = Susi::Util::getFromVar(packet,"payload");
	EXPECT_FALSE(typeVar.isEmpty());
	EXPECT_FALSE(keyVar.isEmpty());
	EXPECT_FALSE(payloadVar.isEmpty());
	EXPECT_TRUE(typeVar.isString());
	EXPECT_TRUE(keyVar.isString());
	EXPECT_TRUE(payloadVar.isString());
	EXPECT_EQ("status",typeVar.convert<std::string>());
	EXPECT_EQ("error",keyVar.convert<std::string>());
	EXPECT_EQ("you are allready subscribed to foobar",payloadVar.convert<std::string>());
}

TEST_F(ApiServerTest,Unsubscribe){
	Poco::Net::SocketAddress addr("[::1]:4000");
	Poco::Net::StreamSocket sock(addr);
	Susi::TCPConnection conn(sock);

	//First subscribe. Should be ok.
	conn.send(Susi::Event::Payload({
		{"type","subscribe"},
		{"key","foobar"}
	}));
	Poco::Dynamic::Var packet;
	bool ok = conn.recv(packet);
	EXPECT_TRUE(ok);
	
	//first unsubscribe. should be ok
	conn.send(Susi::Event::Payload({
		{"type","unsubscribe"},
		{"key","foobar"}
	}));
	ok = conn.recv(packet);
	EXPECT_TRUE(ok);
	auto typeVar = Susi::Util::getFromVar(packet,"type");
	auto keyVar = Susi::Util::getFromVar(packet,"key");
	auto payloadVar = Susi::Util::getFromVar(packet,"payload");
	EXPECT_FALSE(typeVar.isEmpty());
	EXPECT_FALSE(keyVar.isEmpty());
	EXPECT_FALSE(payloadVar.isEmpty());
	EXPECT_TRUE(typeVar.isString());
	EXPECT_TRUE(keyVar.isString());
	EXPECT_TRUE(payloadVar.isString());
	EXPECT_EQ("status",typeVar.convert<std::string>());
	EXPECT_EQ("ok",keyVar.convert<std::string>());
	EXPECT_EQ("successfully unsubscribed from foobar",payloadVar.convert<std::string>());

	//second unsubscribe. should fail
	conn.send(Susi::Event::Payload({
		{"type","unsubscribe"},
		{"key","foobar"}
	}));
	ok = conn.recv(packet);
	EXPECT_TRUE(ok);
	typeVar = Susi::Util::getFromVar(packet,"type");
	keyVar = Susi::Util::getFromVar(packet,"key");
	payloadVar = Susi::Util::getFromVar(packet,"payload");
	EXPECT_FALSE(typeVar.isEmpty());
	EXPECT_FALSE(keyVar.isEmpty());
	EXPECT_FALSE(payloadVar.isEmpty());
	EXPECT_TRUE(typeVar.isString());
	EXPECT_TRUE(keyVar.isString());
	EXPECT_TRUE(payloadVar.isString());
	EXPECT_EQ("status",typeVar.convert<std::string>());
	EXPECT_EQ("error",keyVar.convert<std::string>());
	EXPECT_EQ("you are not subscribed to foobar",payloadVar.convert<std::string>());
}
*/