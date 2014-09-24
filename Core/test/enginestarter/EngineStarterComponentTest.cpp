#include "util/ComponentTest.h"

class EngineStarterComponentTest : public ComponentTest {
public:
	std::string script;
	std::string output;
	Susi::IOController io;
protected:
	
	virtual void SetUp() override {		
		script = base_path+"/test.sh";
		output = base_path+"/test.out";

		io.writeFile(script,"#!/bin/bash\nfor i in $(seq 1 10); do echo -n foobar >> "+output+"\nsleep 0.1; done; exit 0\n");
		io.setExecutable(script,true);

		componentManager->startComponent("enginestarter");
	}
	
	virtual void TearDown() override {
		componentManager->stopComponent("enginestarter");
	}

	virtual void GoodCases() override {
		std::string content;

		Susi::Logger::debug("OUTPUT PATH:"+output);

		//create event
		auto evt = createEvent("enginestarter::start");	
		evt->payload["path"] = base_path;
		publish_sync(std::move(evt));

		std::this_thread::sleep_for(std::chrono::milliseconds(1500));

		content = io.readFile(output);
		EXPECT_TRUE(content.length() > 0);
		io.deletePath(output);

		//start stop
		auto evt2 = createEvent("enginestarter::start");	
		evt2->payload["path"] = base_path;
		publish_sync(std::move(evt2));

		auto evt3 = createEvent("enginestarter::stop");			
		publish_sync(std::move(evt3));

		std::this_thread::sleep_for(std::chrono::milliseconds(1500));

		EXPECT_THROW({
			content = io.readFile(output);
		},std::runtime_error);

		//restart
		auto evt4 = createEvent("enginestarter::start");	
		evt4->payload["path"] = base_path;
		publish_sync(std::move(evt4));

		std::this_thread::sleep_for(std::chrono::milliseconds(500));

		auto evt5 = createEvent("enginestarter::stop");			
		publish_sync(std::move(evt5));

		auto evt6 = createEvent("enginestarter::restart");
		evt6->payload["path"] = base_path;		
		publish_sync(std::move(evt6));

		std::this_thread::sleep_for(std::chrono::milliseconds(1500));

		content = io.readFile(output);	
		EXPECT_TRUE(content.length() > 0);		

	}

	virtual void BadCases() override {

		//bad path
		auto evt = createEvent("enginestarter::start");	
		evt->payload["path"] = base_path + "/dont/exist";
		auto result = publish_sync(std::move(evt));

		EXPECT_TRUE(hasErrorHeader(result));

		//bad path
		auto evt2 = createEvent("enginestarter::restart");	
		evt2->payload["path"] = base_path + "/dont/exist";
		auto result2 = publish_sync(std::move(evt2));

		EXPECT_TRUE(hasErrorHeader(result2));


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
