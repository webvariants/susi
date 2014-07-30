#include "gtest/gtest.h"
#include "tiny-js/JSEngine.h"

TEST(JSEngine, Basic) {
	Susi::JSEngine engine{};
	std::string source = "a=15;b=8;c='abc';a+b+c;";
	std::string result = engine.run(source);
	EXPECT_EQ("23abc",result);
}