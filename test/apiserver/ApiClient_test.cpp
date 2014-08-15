#include "gtest/gtest.h"

#include "apiserver/ApiClient.h"
#include "world/World.h"

class ApiClientTest : public ::testing::Test {
protected:
	virtual void SetUp() override {
		world.setupEventManager();
		world.setupSessionManager();
		world.setupTCPServer();
	}
};

TEST_F(ApiClientTest, Basic){
	using namespace Susi::Events;
	Susi::Api::ApiClient client{"[::1]:4000"};
	bool called1 = false, called2 = false;
	std::condition_variable cond1,cond2;
	std::string topic = "foobar";
	client.subscribe(topic,[&](EventPtr e){
		called1 = true;
		cond1.notify_all();
		client.ack(std::move(e));
	});
	EventPtr event{new Event};
	event->topic=topic;
	client.publish(std::move(event),[&](SharedEventPtr event){
		called2 = true;
		cond2.notify_all();
	});

	/*std::this_thread::sleep_for(std::chrono::milliseconds{250});
	client.close();*/

	std::mutex mutex;
	{
		std::unique_lock<std::mutex> lock(mutex);
		cond1.wait(lock,[&called1](){return called1;}); 
	}
	{
		std::unique_lock<std::mutex> lock(mutex);
		cond2.wait(lock,[&called2](){return called2;}); 
	}
}