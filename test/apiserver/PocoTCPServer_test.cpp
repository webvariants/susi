#include "gtest/gtest.h"

#include "apiserver/PocoTCPServer.h"
#include "world/World.h"

class PocoTCPServerTest : public ::testing::Test {
protected:
	virtual void SetUp() override {
		world.setupEventManager();
	}
};


TEST_F(PocoTCPServerTest,Basic){
	Susi::Api::PocoTCPServer server{12345};
	std::this_thread::sleep_for(std::chrono::seconds{120});
}