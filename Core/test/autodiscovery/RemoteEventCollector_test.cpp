#include "util/ComponentTest.h"
#include "autodiscovery/RemoteEventCollector.h"

class RemoteEventCollectorTest : public ComponentTest {
protected:
	
	virtual void SetUp() override {
		componentManager->startComponent("tcpapiserver");
	}
	
	virtual void TearDown() override {
		componentManager->stopComponent("tcpapiserver");
	}

	virtual void GoodCases() override {
		Susi::Autodiscovery::RemoteEventCollector collector{"[::1]:4000","samplename",eventManager};
		std::this_thread::sleep_for(std::chrono::milliseconds{100});
		std::condition_variable cond;
		bool called = false;
		std::mutex mutex;
		Susi::Events::Consumer consumer{[&](Susi::Events::SharedEventPtr evt){
			called = true;
			cond.notify_one();
		}};
		subscribe("foo",consumer);
		auto event = createEvent("foo@samplename");
		publish(std::move(event));
		{
			std::unique_lock<std::mutex> lock{mutex};
			cond.wait_for(lock,std::chrono::milliseconds{250},[&](){return called;});
		}
		EXPECT_TRUE(called);
	}

	virtual void BadCases() override {
		EXPECT_THROW({
			Susi::Autodiscovery::RemoteEventCollector collector("[::1]:4001","samplename",eventManager);	
		},std::exception);
	}

	virtual void EdgeCases() override {
		EXPECT_THROW({
			Susi::Autodiscovery::RemoteEventCollector collector("[::1]:4000","samplename",nullptr);	
		},std::exception);
	}

};

TEST_F(RemoteEventCollectorTest,GoodCases){
	GoodCases();
}

TEST_F(RemoteEventCollectorTest,BadCases){
	BadCases();
}

TEST_F(RemoteEventCollectorTest,EdgeCases){
	EdgeCases();
}
