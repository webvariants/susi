#include "gtest/gtest.h"
#include "susi/util/ComponentTest.h"
#include "susi/apiserver/ApiClient.h"
#include <mutex>
#include <condition_variable>
#include <vector>
#include <utility>

#include <Poco/Net/SocketReactor.h>
#include <Poco/Net/SocketAcceptor.h>
#include <Poco/Net/SocketNotification.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/NObserver.h>


class ApiClientTest : public ComponentTest {
protected:
	virtual void SetUp() override {
		componentManager->startComponent("tcpapiserver");
	}
	virtual void TearDown() override {
		componentManager->stopAll();
	}

	virtual void GoodCases(){}
	virtual void BadCases(){}
	virtual void EdgeCases(){}

};

TEST_F(ApiClientTest, Basic) {
	std::mutex mutex;
	std::condition_variable cond;
	bool ready = false;

	Susi::Api::ApiClient api{"[::1]:4000"};
	api.subscribe("foo",Susi::Events::Consumer{[&](Susi::Events::SharedEventPtr evt){
		ready = true;
		cond.notify_all();
	}});
	api.publish(api.createEvent("foo"));
	{
		std::unique_lock<std::mutex> lock{mutex};
		cond.wait_for(lock, std::chrono::milliseconds{250}, [&ready](){return ready;});
	}
	EXPECT_TRUE(ready);
}

TEST_F(ApiClientTest, Reconnect) {
	std::mutex mutex;
	std::condition_variable cond;
	bool ready = false;

	Susi::Api::ApiClient api{"[::1]:4000"};
	api.subscribe("foo",Susi::Events::Consumer{[&](Susi::Events::SharedEventPtr evt){
		ready = true;
		cond.notify_all();
	}});
	
	componentManager->unloadAll();
	componentManager->startComponent("tcpapiserver");

	api.publish(api.createEvent("foo"));
	{
		std::unique_lock<std::mutex> lock{mutex};
		cond.wait_for(lock, std::chrono::milliseconds{1000}, [&ready](){return ready;});
	}
	EXPECT_TRUE(ready);
}