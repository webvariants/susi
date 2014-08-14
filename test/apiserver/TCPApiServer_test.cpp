#include "gtest/gtest.h"

#include "apiserver/BasicApiClient.h"
#include "world/World.h"

class TCPApiServerTest : public ::testing::Test {
protected:
	virtual void SetUp() override {
		world.setupEventManager();
		world.setupTCPServer();
	}
};


TEST_F(TCPApiServerTest,Basic){
	using Susi::Util::Any;
	class TestClient : public Susi::Api::BasicApiClient {
	public:
		TestClient(std::string addr) : Susi::Api::BasicApiClient{addr} {}
		std::condition_variable condConsumer,condAck,condClose;
		bool onConnectCalled = false,
		 onConsumerCalled = false,
		 onProcessorCalled = false,
		 onAckCalled = false,
		 onCloseCalled = false;
	protected:
		virtual void onConnect() override {
			onConnectCalled = true;
		}
		virtual void onProcessorEvent(Susi::Events::EventPtr event) override {
			onProcessorCalled = true;
		}
		virtual void onConsumerEvent(Susi::Events::SharedEventPtr event) override {
			onConsumerCalled = true;
			condConsumer.notify_all();
		}
		virtual void onAck(Susi::Events::SharedEventPtr event) override {
			onAckCalled = true;
			condAck.notify_all();
		}
		virtual void onClose() override {
			onCloseCalled = true;
			condClose.notify_all();
		}
	};

	TestClient client{"[::1]:4000"};
	client.sendRegisterConsumer("foobar");
	client.sendRegisterProcessor("foobar");
	Susi::Events::Event event;
	event.topic = "foobar";
	client.sendPublish(event);
	std::this_thread::sleep_for(std::chrono::milliseconds{100});
	client.close();
	std::mutex mutex;
	{
		std::unique_lock<std::mutex> lock(mutex);
		client.condConsumer.wait(lock,[&client](){return client.onConsumerCalled;}); 
	}
	{
		std::unique_lock<std::mutex> lock(mutex);
		client.condAck.wait(lock,[&client](){return client.onAckCalled;}); 
	}
	{
		std::unique_lock<std::mutex> lock(mutex);
		client.condClose.wait(lock,[&client](){return client.onCloseCalled;}); 
	}

	EXPECT_TRUE(client.onConnectCalled);
	EXPECT_TRUE(client.onConsumerCalled);
	EXPECT_TRUE(client.onProcessorCalled);
	EXPECT_TRUE(client.onAckCalled);
	EXPECT_TRUE(client.onCloseCalled);

}