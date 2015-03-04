#include "susi/util/ComponentTest.h"

class HeartBeatComponentTest : public ComponentTest {
protected:
	
	virtual void SetUp() override {
		componentManager->startComponent("heartbeat");
	}
	
	virtual void TearDown() override {
		componentManager->stopComponent("heartbeat");
	}

	virtual void GoodCases() override {
		auto event = waitForConsumerEvent("heartbeat::one",std::chrono::milliseconds{1100});
		EXPECT_TRUE(event.get()!=nullptr);
	}

	virtual void BadCases() override {
		auto event = waitForConsumerEvent("heartbeat::wrong",std::chrono::milliseconds{1100});
		EXPECT_TRUE(event.get()==nullptr);
	}

	virtual void EdgeCases() override {
		auto event = waitForProcessorEvent("heartbeat::one",std::chrono::milliseconds{1100});
		EXPECT_TRUE(event.get()!=nullptr);
	}

};

TEST_F(HeartBeatComponentTest,GoodCases){
	GoodCases();
}

TEST_F(HeartBeatComponentTest,BadCases){
	BadCases();
}

TEST_F(HeartBeatComponentTest,EdgeCases){
	EdgeCases();
}
