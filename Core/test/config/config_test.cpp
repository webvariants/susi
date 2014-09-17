#include "gtest/gtest.h"
#include "world/ComponentManager.h"
#include "world/system_setup.h"
#include "config/Config.h"
#include "iocontroller/IOController.h"

class ConfigTest : public ::testing::Test {

public:
   	std::string base_path = Poco::Path(Poco::Path::current()).toString() + "configtest/";
   	Susi::IOController io;

protected:
	virtual void SetUp() override {
		io.makeDir(base_path);
	}
	virtual void TearDown() override {
		io.deletePath(base_path);
	}
};

TEST_F(ConfigTest, Contruct) {
	Susi::Util::Any cfg = Susi::Util::Any::Object{
		{"foo","bar"}
	};
	// Test valid json
	io.writeFile(base_path + "config.json",cfg.toJSONString());
	EXPECT_NO_THROW({
		auto config = std::make_shared<Susi::Config>(base_path + "config.json");
	});
}

TEST_F(ConfigTest, ContructInvalidJson) {
	io.writeFile(base_path + "config.json","{\"foo\",\"bar\", 22");
	EXPECT_THROW({
		auto config = std::make_shared<Susi::Config>(base_path + "config.json");
	},std::runtime_error);
}


TEST_F(ConfigTest, ContructWrongFormatJson) {
	// Test valid json which is no object;
	io.writeFile(base_path + "config.json","\"wrongformat\"");
	EXPECT_THROW({
		auto config = std::make_shared<Susi::Config>(base_path + "config.json");
	},std::runtime_error);
}

TEST_F(ConfigTest, ContructConfigMissing) {
	// Test nonexistent file;
	EXPECT_THROW({
		auto config = std::make_shared<Susi::Config>(base_path + "wrongname.json");
	},std::runtime_error);
}


TEST_F(ConfigTest, Get){
	using Susi::Util::Any;
	Any cfg = Any::Object{{"foo",Any::Object{{"bar", Any::Object{{"baz",123}}}}}};

	io.writeFile(base_path + "config.json",cfg.toJSONString());
	EXPECT_NO_THROW({
		auto config = std::make_shared<Susi::Config>(base_path + "config.json");
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
		auto config = std::make_shared<Susi::Config>(base_path + "config.json");
		config->get("bla");
	},std::runtime_error);

	EXPECT_THROW({
		auto config = std::make_shared<Susi::Config>(base_path + "config.json");
		config->get("bla.blub");
	},std::runtime_error);

}

TEST_F(ConfigTest,CommandLine){
	using Susi::Util::Any;
	Any cfg = Any::Object{{"foo",Any::Object{{"bar", Any::Object{{"baz",123}}}}}};
	io.writeFile(base_path + "config.json",cfg.toJSONString());

	std::vector<std::string> cmdLine_1 = {"prognameIsAllwaysFirstParam","-baz","321"};
	std::vector<std::string> cmdLine_2 = {"prognameIsAllwaysFirstParam","-baz","321.123"};
	std::vector<std::string> cmdLine_3 = {"prognameIsAllwaysFirstParam","-baz","this is it"};

	EXPECT_NO_THROW({
		auto config = std::make_shared<Susi::Config>(base_path + "config.json");
		config->registerCommandLineOption("baz","foo.bar.baz");
		config->parseCommandLine(cmdLine_1);
		EXPECT_EQ(Any{321}.toJSONString(),config->get("foo.bar.baz").toJSONString());


		config->parseCommandLine(cmdLine_2);
		EXPECT_EQ(Any{321.123}.toJSONString(),config->get("foo.bar.baz").toJSONString());


		config->parseCommandLine(cmdLine_3);
		EXPECT_EQ(Any{"this is it"}.toJSONString(),config->get("foo.bar.baz").toJSONString());

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
	EXPECT_EQ("\"bla\"",cfg.get("foo").toJSONString());
}

TEST_F(ConfigTest,CommandLineOneDashOneArg2){
	Susi::Config cfg;
	std::vector<std::string> cmdline = {"prognameIsAllwaysFirstParam","-foo=bla"};
	cfg.parseCommandLine(cmdline);
	EXPECT_TRUE(cfg.get("foo").isString());
	EXPECT_EQ("\"bla\"",cfg.get("foo").toJSONString());
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
	EXPECT_EQ("\"bla\"",cfg.get("foo").toJSONString());
}

TEST_F(ConfigTest,CommandLineTwoDashOneArg2){
	Susi::Config cfg;
	std::vector<std::string> cmdline = {"prognameIsAllwaysFirstParam","--foo=bla"};
	cfg.parseCommandLine(cmdline);
	EXPECT_TRUE(cfg.get("foo").isString());
	EXPECT_EQ("\"bla\"",cfg.get("foo").toJSONString());
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
	EXPECT_EQ("\"bla\"",cfg.get("foo").toJSONString());
	EXPECT_EQ("\"bla\"",cfg.get("bar").toJSONString());
	EXPECT_TRUE(cfg.get("bla").isObject());
	EXPECT_EQ("\"bla\"",cfg.get("baz").toJSONString());
}

TEST_F(ConfigTest,MultiLevelArguments){
	Susi::Config cfg;
	std::vector<std::string> cmdline = {"prognameIsAllwaysFirstParam","--foo.bar.baz=bla"};
	cfg.parseCommandLine(cmdline);
	EXPECT_TRUE(cfg.get("foo.bar.baz").isString());
	EXPECT_EQ("\"bla\"",cfg.get("foo.bar.baz").toJSONString());
}

TEST_F(ConfigTest,MultiConfigSupport){

	Susi::Util::Any cfg_1 = Susi::Util::Any::Object{
		{"foo","bar"},
		{"data", "test1"}
	};

	// Test valid json
	io.writeFile(base_path+ "config_1.json",cfg_1.toJSONString());

	Susi::Util::Any cfg_2 = Susi::Util::Any::Object{
		{"john","doe"},
		{"data", "test2"}
	};

	io.writeFile(base_path+ "config_2.json",cfg_2.toJSONString());

	Susi::Config cfg;

	cfg.loadConfig(base_path+ "config_1.json");
	cfg.loadConfig(base_path+ "config_2.json");


	Susi::Util::Any conf = cfg.getConfig();

	EXPECT_TRUE(cfg.get("foo").isString());
	EXPECT_EQ("\"bar\"",cfg.get("foo").toJSONString());

	EXPECT_TRUE(cfg.get("john").isString());
	EXPECT_EQ("\"doe\"",cfg.get("john").toJSONString());

	// test override
	EXPECT_TRUE(cfg.get("data").isString());
	EXPECT_EQ("\"test2\"",cfg.get("data").toJSONString());

}

TEST_F(ConfigTest, LoadAllStartStop){
	// make test independed from config file

	class C1 : public Susi::System::Component {
		public:
			virtual void start() override {}
			virtual void stop() override {}
	};
	class C2 : public C1 {};
	class C3 : public C1 {};

    std::string test_config = "{"
		"		\"c1\" : {},"
		"		\"c2\" : {},"
		"		\"c3\" : {}"
		"	}";

	Susi::Util::Any::Object config = Susi::Util::Any::fromString(test_config);

	auto manager = std::make_shared<Susi::System::ComponentManager>(config);

	manager->registerComponent("c1",[](Susi::System::ComponentManager * mgr, Susi::Util::Any & config){ return std::shared_ptr<Susi::System::Component>{new C1{}};});
	manager->registerComponent("c2",[](Susi::System::ComponentManager * mgr, Susi::Util::Any & config){ return std::shared_ptr<Susi::System::Component>{new C2{}};});
	manager->registerComponent("c3",[](Susi::System::ComponentManager * mgr, Susi::Util::Any & config){ return std::shared_ptr<Susi::System::Component>{new C3{}};});


	bool start = manager->startAll();
	bool stop  = manager->stopAll();

	EXPECT_TRUE(start);
	EXPECT_TRUE(stop);
	/*
	Susi::Config cfg{};
	cfg.loadConfig("config.json");

	std::shared_ptr<Susi::System::ComponentManager> componentManager = Susi::System::createSusiComponentManager(cfg.getConfig());

	bool start = componentManager->startAll();
	bool stop  = componentManager->stopAll();

	EXPECT_TRUE(start);
	EXPECT_TRUE(stop);
	*/
}

TEST_F(ConfigTest, LoadConfigsFromDir){
	Susi::Config cfg;

	Susi::Util::Any cfg_content = Susi::Util::Any::Object{
		{"foo","bar"}
	};

	io.makeDir(base_path+ "sub");
	io.makeDir(base_path+ "sub/sub");
	// Test valid json
	io.writeFile(base_path+ "config.json",cfg_content.toJSONString());
	io.writeFile(base_path+ "config2.json",cfg_content.toJSONString());
	io.writeFile(base_path+ "sub/config3.json",cfg_content.toJSONString());
	io.writeFile(base_path+ "sub/sub/config4.json",cfg_content.toJSONString());


	cfg.setLoadCount(0);
	cfg.loadConfig(base_path);

	EXPECT_EQ(4,cfg.getLoadCount());
}
