#include <gtest/gtest.h>
#include "JSEngine.h"
#include "apiserver/ApiClient.h"
#include <thread>
#include <chrono>

TEST(V8EngineTest, Publish) {
	Susi::Api::ApiClient susi_api{"[::1]:4000"};
	Susi::JS::V8::Engine engine;
	std::condition_variable cond;
	std::mutex mutex;
	bool success = false;
	Susi::Events::Consumer testConsumer = [&cond, &success] (Susi::Events::SharedEventPtr event) {
		success = event->getPayload()["success"];
		cond.notify_one();
	};
	susi_api.subscribe("jstest::basic", testConsumer);
	std::thread t{[&engine](){
		engine.run("publish({topic: 'jstest::basic', payload: {success: true}});");
	}};

	{
		std::unique_lock<std::mutex> lock{mutex};
		cond.wait(lock, [&success](){return success;});
		EXPECT_EQ(true, success);
	}

	std::this_thread::sleep_for(std::chrono::milliseconds{250});
	engine.stop();
	t.join();
}

TEST(V8EngineTest, Publish2) {
	Susi::Api::ApiClient susi_api{"[::1]:4000"};
	Susi::JS::V8::Engine engine;
	std::condition_variable cond;
	std::mutex mutex;
	bool success = false;
	Susi::Events::Consumer testConsumer = [&cond, &success] (Susi::Events::SharedEventPtr event) {
		success = event->getPayload()["success"];
		cond.notify_one();
	};
	susi_api.subscribe("jstest::basic", testConsumer);
	std::thread t{[&engine](){
		engine.run("publish({topic: 'jstest::basic', payload: {success: true}});");
	}};

	{
		std::unique_lock<std::mutex> lock{mutex};
		cond.wait(lock, [&success](){return success;});
		EXPECT_EQ(true, success);
	}

	std::this_thread::sleep_for(std::chrono::milliseconds{250});
	engine.stop();
	t.join();
}

