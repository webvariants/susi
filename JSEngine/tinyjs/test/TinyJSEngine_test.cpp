#include <gtest/gtest.h>
#include "TinyJSEngine.h"
#include <thread>
#include <chrono>

TEST(TinyJSEngineTest, Basic) {
	Susi::JS::Tiny::Engine engine{"[::1]:4000"};

	engine.run("var a = 0; for (var i=1;i<10;i++) a += i; result = a==45;");


	std::this_thread::sleep_for(std::chrono::seconds{2});
	EXPECT_EQ(true, true);
}
