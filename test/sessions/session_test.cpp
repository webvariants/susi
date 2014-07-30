#include <gtest/gtest.h>
#include <Poco/Dynamic/Var.h>
#include "sessions/Session.h"

class SessionTest : public ::testing::Test {
protected:
	Susi::Session session;
};

TEST_F(SessionTest, Init) {
	EXPECT_FALSE(session.isDead());
}

TEST_F(SessionTest, Attribs) {
	// read value that does not exist
	EXPECT_TRUE(session.getAttribute("super").isEmpty());
	// set value to string
	session.setAttribute("real", "testResult");
	// read just set value
	auto result = session.getAttribute("real");
	EXPECT_FALSE(result.isEmpty());
	EXPECT_EQ("testResult", result.convert<std::string>());

	// reset value
	session.setAttribute("real", "new result");
	// read just reset value
	auto result1 = session.getAttribute("real");
	EXPECT_FALSE(result1.isEmpty());
	EXPECT_EQ("new result", result1.convert<std::string>());

	// set value to bool
	session.setAttribute("boolean", true);
	// read just set value
	auto result2 = session.getAttribute("boolean");
	EXPECT_FALSE(result2.isEmpty());
	EXPECT_EQ(true, result2.convert<bool>());

	// set value to array
	std::vector<int> array = {1,2,3,4};
	session.setAttribute("array", array);
	// read just set value
	auto result3 = session.getAttribute("array");
	EXPECT_FALSE(result3.isEmpty());
	EXPECT_EQ(array,result3.extract<std::vector<int>>());

	// push an element to an array attribute
	session.pushAttribute("array", 5);
	auto result4 = session.getAttribute("array");
	EXPECT_FALSE(result4.isEmpty());
	EXPECT_EQ(array,result4[0].extract<std::vector<int>>());
	EXPECT_EQ(5,result4[1].convert<int>());

	// push an element to an attribute
	std::vector<Poco::Dynamic::Var> array2 = {"new result", 5};
	session.pushAttribute("real", 5);
	auto result5 = session.getAttribute("real");
	EXPECT_FALSE(result5.isEmpty());
	EXPECT_EQ(array2[0].convert<std::string>(),result5[0].convert<std::string>());
	EXPECT_EQ(array2[1].convert<int>(),result5[1].convert<int>());

	// set value with empty string as key
	session.setAttribute("", "empty Test");
	// try to get value of empty string as key
	auto result6 = session.getAttribute("");
	EXPECT_TRUE(result6.isEmpty());
}

TEST_F(SessionTest, Die) {
	auto session = Susi::Session();
	session.die();
	EXPECT_TRUE(session.isDead());
}
