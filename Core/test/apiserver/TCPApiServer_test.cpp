/*

#include "gtest/gtest.h"

#include "apiserver/BasicApiClient.h"
#include "world/World.h"

class TCPApiServerTest : public ::testing::Test {
protected:
	virtual void SetUp() override {
		world.setup();
	}
};


TEST_F(TCPApiServerTest,Basic){
	using Susi::Util::Any;
	class TestClient : public Susi::Api::BasicApiClient {
	public:
		TestClient(std::string addr) : Susi::Api::BasicApiClient{addr} {}
		virtual ~TestClient(){}
		std::condition_variable condConsumer,condAck,condClose;
		bool onConnectCalled = false,
		 onConsumerCalled = false,
		 onProcessorCalled = false,
		 onAckCalled = false,
		 onCloseCalled = false;
	protected:
		virtual void onConnect() override {
			onConnectCalled = true;
			std::cout<<"on connect!"<<std::endl;
			sendRegisterConsumer("foobar");
			sendRegisterProcessor("foobar");
			Susi::Events::Event event{"foobar"};
			sendPublish(event);
			std::cout<<"sended publish"<<std::endl;
			
		}
		virtual void onProcessorEvent(Susi::Events::Event & event) override {
			std::cout<<"on proc!"<<std::endl;
			onProcessorCalled = true;
			sendAck(event);
		}
		virtual void onConsumerEvent(Susi::Events::Event & event) override {
			onConsumerCalled = true;
			std::cout<<"on consumer!"<<std::endl;
			condConsumer.notify_all();
		}
		virtual void onAck(Susi::Events::Event & event) override {
			onAckCalled = true;
			std::cout<<"on ack!"<<std::endl;
			condAck.notify_all();
		}
		virtual void onClose() override {
			onCloseCalled = true;
			std::cout<<"on close!"<<std::endl;
			condClose.notify_all();
		}
	};

	TestClient client{"[::1]:4000"};
	std::mutex mutex;
	{
		std::unique_lock<std::mutex> lock(mutex);
		client.condConsumer.wait(lock,[&client](){return client.onConsumerCalled;}); 
		std::cout<<"got condition!"<<std::endl;
	}
	{
		std::unique_lock<std::mutex> lock(mutex);
		client.condAck.wait(lock,[&client](){return client.onAckCalled;}); 
		std::cout<<"got condition!"<<std::endl;
	}
	client.close();
	{
		std::unique_lock<std::mutex> lock(mutex);
		client.condClose.wait(lock,[&client](){return client.onCloseCalled;}); 
		std::cout<<"got condition!"<<std::endl;
	}

	EXPECT_TRUE(client.onConnectCalled);
	EXPECT_TRUE(client.onConsumerCalled);
	EXPECT_TRUE(client.onProcessorCalled);
	EXPECT_TRUE(client.onAckCalled);
	EXPECT_TRUE(client.onCloseCalled);

}

*/