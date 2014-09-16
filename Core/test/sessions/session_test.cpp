#include <gtest/gtest.h>
//#include <Poco/Dynamic/Var.h>
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
	EXPECT_TRUE(session.getAttribute("super").isNull());
	// set value to string
	session.setAttribute("real", "testResult");
	// read just set value
	auto result = session.getAttribute("real");
	EXPECT_FALSE(result.isNull());

	std::string resultStr = result;
	EXPECT_EQ("testResult", resultStr);

	// reset value
	session.setAttribute("real", "new result");
	// read just reset value
	auto result1 = session.getAttribute("real");
	EXPECT_FALSE(result1.isNull());

	std::string resultStr1 = result1;
	EXPECT_EQ("new result", resultStr1);

	// set value to bool
	session.setAttribute("boolean", true);
	// read just set value
	auto result2 = session.getAttribute("boolean");
	EXPECT_FALSE(result2.isNull());

	bool result2_bool = result2;
	EXPECT_EQ(true, result2_bool);

	// set value to array
	Susi::Util::Any array{Susi::Util::Any::Array{1,2,3,4}};
	session.setAttribute("array", array);
	// read just set value
	auto result3 = session.getAttribute("array");
	EXPECT_FALSE(result3.isNull());
	EXPECT_EQ(array,result3);

	// push an element to an array attribute
	session.pushAttribute("array", 5);
	auto result4 = session.getAttribute("array");
	EXPECT_FALSE(result4.isNull());

	Susi::Util::Any array4{Susi::Util::Any::Array{Susi::Util::Any::Array{1,2,3,4},Susi::Util::Any{5}}};
	EXPECT_EQ(array4.toJSONString(),result4.toJSONString());

	int result4_int = result4[1];
	EXPECT_EQ(5,result4_int);

	// push an element to an attribute
	std::vector<Susi::Util::Any> array2 = {"new result", 5};
	session.pushAttribute("real", 5);
	auto result5 = session.getAttribute("real");
	EXPECT_FALSE(result5.isNull());
	EXPECT_EQ(array2[0].toJSONString(),result5[0].toJSONString());
	EXPECT_EQ(array2[1].toJSONString(),result5[1].toJSONString());

	// set value with empty string as key
	session.setAttribute("", "empty Test");
	// try to get value of empty string as key
	auto result6 = session.getAttribute("");
	EXPECT_TRUE(result6.isNull());
}

TEST_F(SessionTest, Die) {
	auto session = Susi::Session();
	session.die();
	EXPECT_TRUE(session.isDead());
}
