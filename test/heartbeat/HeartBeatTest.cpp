#include "gtest/gtest.h"
#include "heartbeat/HeartBeat.h"
#include "world/World.h"

class HeartBeatTest : public ::testing::Test {
	public:
	protected:
		virtual void SetUp() override {
			world.setupEventManager();
			world.setupHeartBeat();
		}
		virtual void TearDown() override {
			world.tearDown();
		}
};

TEST_F(HeartBeatTest,Lifecycle){
	
}


TEST_F(HeartBeatTest,Lifecycle2){
	
}

