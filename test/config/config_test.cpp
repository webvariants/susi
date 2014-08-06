#include "gtest/gtest.h"
#include "config/Config.h"
#include "iocontroller/IOController.h"

class ConfigTest : public ::testing::Test {
protected:
	Susi::IOController io;
	virtual void SetUp() override {
		io.makeDir("./configtest/");
	}
	virtual void TearDown() override {
		io.deletePath("./configtest/");
	}
};

TEST_F(ConfigTest, Contruct) {
	Susi::Util::Any cfg = Susi::Util::Any::Object{
		{"foo","bar"}
	};
	// Test valid json
	io.writeFile("./configtest/config.cfg",cfg.toString());
	EXPECT_NO_THROW({
		auto config = std::make_shared<Susi::Config>("./configtest/config.cfg");
	});
	// Test invalid json
	io.writeFile("./configtest/config.cfg",cfg.toString()+"wrongformat");
	EXPECT_THROW({
		auto config = std::make_shared<Susi::Config>("./configtest/config.cfg");
	},std::runtime_error);
	// Test valid json which is no object;
	io.writeFile("./configtest/config.cfg","\"wrongformat\"");
	EXPECT_THROW({
		auto config = std::make_shared<Susi::Config>("./configtest/config.cfg");
	},std::runtime_error);
	// Test nonexistent file;
	EXPECT_THROW({
		auto config = std::make_shared<Susi::Config>("./configtest/wrongname.cfg");
	},std::runtime_error);
}

TEST_F(ConfigTest, Get){
	using Susi::Util::Any;
	Any cfg = Any::Object{{"foo",Any::Object{{"bar", Any::Object{{"baz",123}}}}}};
	io.writeFile("./configtest/config.cfg",cfg.toString());
	EXPECT_NO_THROW({
		auto config = std::make_shared<Susi::Config>("./configtest/config.cfg");
		Any value = config.get("");
		EXPECT_EQ(cfg,value);
		Any value = config.get("foo");
		EXPECT_EQ(cfg["foo"],value);
		Any value = config.get("foo.bar");
		EXPECT_EQ(cfg["foo"]["bar"],value);
		Any value = config.get("foo.bar.baz");
		EXPECT_EQ(cfg["foo"]["bar"]["baz"],value);
	});
	EXPECT_THROW({
		auto config = std::make_shared<Susi::Config>("./configtest/config.cfg");
		config.get("bla");
	},std::runtime_error);
	EXPECT_THROW({
		auto config = std::make_shared<Susi::Config>("./configtest/config.cfg");
		config.get("bla.blub");
	},std::runtime_error);
}

TEST_F(ConfigTest,CommandLine){
	using Susi::Util::Any;
	Any cfg = Any::Object{{"foo",Any::Object{{"bar", Any::Object{{"baz",123}}}}}};
	io.writeFile("./configtest/config.cfg",cfg.toString());
	EXPECT_NO_THROW({
		auto config = std::make_shared<Susi::Config>("./configtest/config.cfg");
		config.registerCommandLineOption("baz","foo.bar.baz");
		char ** cmdLine = {"prognameIsAllwaysFirstParam","-baz",321}
		config.parseCommandLine(3,cmdLine);
		EXPECT_EQ(Any{321},config.get("foo.bar.baz"));
		char ** cmdLine = {"prognameIsAllwaysFirstParam","-baz",321.123}
		config.parseCommandLine(3,cmdLine);
		EXPECT_EQ(Any{321.123},config.get("foo.bar.baz"));
		char ** cmdLine = {"prognameIsAllwaysFirstParam","-baz","this is it"}
		config.parseCommandLine(3,cmdLine);
		EXPECT_EQ(Any{"this is it"},config.get("foo.bar.baz"));
	});
	EXPECT_THROW({
		auto config = std::make_shared<Susi::Config>("./configtest/config.cfg");
		char ** cmdLine = {"prognameIsAllwaysFirstParam","-baz",321}
	},std::runtime_error);
}
