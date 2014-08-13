#include "gtest/gtest.h"

#include "apiserver/JSONTCPServer.h"
#include "apiserver/JSONTCPClient.h"

#include <mutex>
#include <condition_variable>


class JSONTCPServerClientTest : public ::testing::Test {
protected:
	class EchoServer : public Susi::Api::JSONTCPServer {
	public:
		std::mutex mutex;
		std::condition_variable cond;

		bool onConnectCalled = false;
		bool onDataCalled = false;
		bool onCloseCalled = false;

		EchoServer(unsigned short port) : Susi::Api::JSONTCPServer{port} {}

		virtual void onConnect(std::string & id) override {
			Susi::Api::JSONTCPServer::onConnect(id);
			onConnectCalled = true;
		}
		virtual void onMessage(std::string & id, Susi::Util::Any & data) override {
			onDataCalled = true;
			send(id,data);
		}
		virtual void onClose(std::string & id) override {
			Susi::Api::JSONTCPServer::onClose(id);
			onCloseCalled = true;
			cond.notify_all();
			stop();
		}
	};

	class EchoClient : public Susi::Api::JSONTCPClient {
	public:
		bool onConnectCalled = false;
		bool onDataCalled = false;
		bool onCloseCalled = false;

		std::mutex mutex;
		std::condition_variable cond;

		EchoClient(std::string host, unsigned short port) : Susi::Api::JSONTCPClient{host,port} {}
	protected:
		virtual void onConnect() override{
			Susi::Api::JSONTCPClient::onConnect();
			onConnectCalled = true;
			using Susi::Util::Any;
			Any data = Any::Object{
				{"foo","bar"}
			};
			send(data);
		}
		virtual void onMessage(Susi::Util::Any & data) override{
			onDataCalled = true;
			using Susi::Util::Any;
			Any shouldBeLikeThis = Any::Object{
				{"foo","bar"}
			};
			EXPECT_EQ(shouldBeLikeThis,data);
			close();
		}
		virtual void onClose() override{
			Susi::Api::JSONTCPClient::onClose();
			onCloseCalled = true;
			cond.notify_all();
		}
	};
};


TEST_F(JSONTCPServerClientTest,Echo){
	EchoServer server{12346};
	EchoClient client{"localhost",12346};
	{
		std::unique_lock<std::mutex> lock(client.mutex);
		client.cond.wait(lock,[&client](){return client.onCloseCalled;}); 
	}
	{
		std::unique_lock<std::mutex> lock(server.mutex);
		server.cond.wait(lock,[&server](){return server.onCloseCalled;}); 
	}
	EXPECT_TRUE(server.onConnectCalled);
	EXPECT_TRUE(server.onDataCalled);
	EXPECT_TRUE(server.onCloseCalled);

	EXPECT_TRUE(client.onConnectCalled);
	EXPECT_TRUE(client.onDataCalled);
	EXPECT_TRUE(client.onCloseCalled);
}