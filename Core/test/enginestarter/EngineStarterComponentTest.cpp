#include "util/ComponentTest.h"

class EngineStarterComponentTest : public ComponentTest {
public:
	std::string base_path;
	Susi::IOController io;
protected:
	
	virtual void SetUp() override {
		base_path = Poco::Path(Poco::Path::current()).toString() + "enginestarter_test/";
		io = Susi::IOController{base_path};
		io.makeDir(base_path);

		std::string script = base_path+"test.sh";
		std::string output = base_path+"test.out";

		io.writeFile(script,"#!/bin/bash\nfor i in $(seq 1 10); do echo -n foobar >> "+output+"\nsleep 0.1; done; exit 0\n");
		io.setExecutable(script,true);

		componentManager->startComponent("enginestarter");
	}
	
	virtual void TearDown() override {
		//io.deletePath(base_path);
		componentManager->stopComponent("enginestarter");
	}

	virtual void GoodCases() override {
		//create event
		auto evt = createEvent("enginestarter::start");	
		evt->payload["path"] = base_path;
		auto result = publish_sync(std::move(evt));

		std::this_thread::sleep_for(std::chrono::milliseconds(1500));

		//check event
		//EXPECT_TRUE(static_cast<bool>(result->payload["success"]));

	}

	virtual void BadCases() override {

	}

	virtual void EdgeCases() override {

	}

};

TEST_F(EngineStarterComponentTest,GoodCases){
	GoodCases();
}

TEST_F(EngineStarterComponentTest,BadCases){
	BadCases();
}

TEST_F(EngineStarterComponentTest,EdgeCases){
	EdgeCases();
}
