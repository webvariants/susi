#include <gtest/gtest.h>
#include "JSEngine.h"
#include <thread>
#include <chrono>

TEST(JSEngineTest, Basic) {
	Susi::JS::Engine engine{"[::1]:4000","../bin/test.js"};
	std::this_thread::sleep_for(std::chrono::seconds{2});
	EXPECT_EQ(true, true);
}
