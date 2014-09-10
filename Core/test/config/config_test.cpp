#include "gtest/gtest.h"
#include "world/ComponentManager.h"
#include "world/system_setup.h"
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
	//io.writeFile("./configtest/config.cfg",cfg.toString()+"wrongformat");
	io.writeFile("./configtest/config.cfg","{\"foo\",\"bar\", 22");
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
	std::cout<<"CO:"<<cfg.toString()<<std::endl;
	io.writeFile("./configtest/config.cfg",cfg.toString());
	EXPECT_NO_THROW({
		auto config = std::make_shared<Susi::Config>("./configtest/config.cfg");
		Any value1 = config->get("");
		EXPECT_EQ(cfg,value1);
		Any value2 = config->get("foo");		
		EXPECT_EQ(cfg["foo"],value2);	
		Any value3 = config->get("foo.bar");
		EXPECT_EQ(cfg["foo"]["bar"],value3);
		Any value4 = config->get("foo.bar.baz");
		EXPECT_EQ(cfg["foo"]["bar"]["baz"],value4);
	});
	
	EXPECT_THROW({
		auto config = std::make_shared<Susi::Config>("./configtest/config.cfg");
		config->get("bla");
	},std::runtime_error);

	EXPECT_THROW({
		auto config = std::make_shared<Susi::Config>("./configtest/config.cfg");
		config->get("bla.blub");
	},std::runtime_error);

}

TEST_F(ConfigTest,CommandLine){
	using Susi::Util::Any;
	Any cfg = Any::Object{{"foo",Any::Object{{"bar", Any::Object{{"baz",123}}}}}};
	io.writeFile("./configtest/config.cfg",cfg.toString());

	std::vector<std::string> cmdLine_1 = {"prognameIsAllwaysFirstParam","-baz","321"};
	std::vector<std::string> cmdLine_2 = {"prognameIsAllwaysFirstParam","-baz","321.123"};
	std::vector<std::string> cmdLine_3 = {"prognameIsAllwaysFirstParam","-baz","this is it"};

	EXPECT_NO_THROW({
		auto config = std::make_shared<Susi::Config>("./configtest/config.cfg");
		config->registerCommandLineOption("baz","foo.bar.baz");		
		config->parseCommandLine(cmdLine_1);		
		EXPECT_EQ(Any{321}.toString(),config->get("foo.bar.baz").toString());

				
		config->parseCommandLine(cmdLine_2);
		EXPECT_EQ(Any{321.123}.toString(),config->get("foo.bar.baz").toString());
		

		config->parseCommandLine(cmdLine_3);
		EXPECT_EQ(Any{"this is it"}.toString(),config->get("foo.bar.baz").toString());
		
	});
}

TEST_F(ConfigTest,CommandLineOneDashNoArg){
	Susi::Config cfg;
	std::vector<std::string> cmdline = {"prognameIsAllwaysFirstParam","-foo"};
	cfg.parseCommandLine(cmdline);
	EXPECT_TRUE(cfg.get("foo").isObject());		
}

TEST_F(ConfigTest,CommandLineOneDashOneArg){
	Susi::Config cfg;
	std::vector<std::string> cmdline = {"prognameIsAllwaysFirstParam","-foo","bla"};
	cfg.parseCommandLine(cmdline);
	EXPECT_TRUE(cfg.get("foo").isString());
	EXPECT_EQ("\"bla\"",cfg.get("foo").toString());		
}

TEST_F(ConfigTest,CommandLineOneDashOneArg2){
	Susi::Config cfg;
	std::vector<std::string> cmdline = {"prognameIsAllwaysFirstParam","-foo=bla"};
	cfg.parseCommandLine(cmdline);
	EXPECT_TRUE(cfg.get("foo").isString());
	EXPECT_EQ("\"bla\"",cfg.get("foo").toString());		
}

TEST_F(ConfigTest,CommandLineTwoDashNoArg){
	Susi::Config cfg;
	std::vector<std::string> cmdline = {"prognameIsAllwaysFirstParam","--foo"};
	cfg.parseCommandLine(cmdline);
	EXPECT_TRUE(cfg.get("foo").isObject());		
}

TEST_F(ConfigTest,CommandLineTwoDashOneArg){
	Susi::Config cfg;
	std::vector<std::string> cmdline = {"prognameIsAllwaysFirstParam","--foo","bla"};
	cfg.parseCommandLine(cmdline);
	EXPECT_TRUE(cfg.get("foo").isString());
	EXPECT_EQ("\"bla\"",cfg.get("foo").toString());		
}

TEST_F(ConfigTest,CommandLineTwoDashOneArg2){
	Susi::Config cfg;
	std::vector<std::string> cmdline = {"prognameIsAllwaysFirstParam","--foo=bla"};
	cfg.parseCommandLine(cmdline);
	EXPECT_TRUE(cfg.get("foo").isString());
	EXPECT_EQ("\"bla\"",cfg.get("foo").toString());		
}

TEST_F(ConfigTest,MultipleArguments){
	Susi::Config cfg;
	std::vector<std::string> cmdline = {
		"prognameIsAllwaysFirstParam",
		"--foo=bla",
		"-bar","bla",
		"-bla",
		"--baz","bla"};
	cfg.parseCommandLine(cmdline);
	EXPECT_EQ("\"bla\"",cfg.get("foo").toString());
	EXPECT_EQ("\"bla\"",cfg.get("bar").toString());
	EXPECT_TRUE(cfg.get("bla").isObject());		
	EXPECT_EQ("\"bla\"",cfg.get("baz").toString());	
}

TEST_F(ConfigTest,MultiLevelArguments){
	Susi::Config cfg;
	std::vector<std::string> cmdline = {"prognameIsAllwaysFirstParam","--foo.bar.baz=bla"};
	cfg.parseCommandLine(cmdline);
	EXPECT_TRUE(cfg.get("foo.bar.baz").isString());
	EXPECT_EQ("\"bla\"",cfg.get("foo.bar.baz").toString());			
}

TEST_F(ConfigTest,MultiConfigSupport){

	Susi::Util::Any cfg_1 = Susi::Util::Any::Object{
		{"foo","bar"},
		{"data", "test1"}
	};

	// Test valid json
	io.writeFile("./configtest/config_1.cfg",cfg_1.toString());	

	Susi::Util::Any cfg_2 = Susi::Util::Any::Object{
		{"john","doe"},
		{"data", "test2"}
	};

	io.writeFile("./configtest/config_2.cfg",cfg_2.toString());
	
	Susi::Config cfg;

	cfg.loadConfig("./configtest/config_1.cfg");
	cfg.loadConfig("./configtest/config_2.cfg");


	Susi::Util::Any conf = cfg.getConfig();

	std::cout<<"RESULT: "<<conf.toString()<<std::endl;
	
	
	EXPECT_TRUE(cfg.get("foo").isString());
	EXPECT_EQ("\"bar\"",cfg.get("foo").toString());

	EXPECT_TRUE(cfg.get("john").isString());
	EXPECT_EQ("\"doe\"",cfg.get("john").toString());

	// test override
	EXPECT_TRUE(cfg.get("data").isString());
	EXPECT_EQ("\"test2\"",cfg.get("data").toString());
	
}

TEST_F(ConfigTest, LoadAllStartStop){

	Susi::Config cfg{};
	cfg.loadConfig("config.json");

	std::shared_ptr<Susi::System::ComponentManager> componentManager = Susi::System::createSusiComponentManager(cfg.getConfig());

	bool start = componentManager->startAll();
	bool stop  = componentManager->stopAll();

	EXPECT_TRUE(start);
	EXPECT_TRUE(stop);	
}