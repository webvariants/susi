#include "util/ComponentTest.h"

class IOControllerComponentTest : public ComponentTest {

public:
	std::string base_path;
protected:
	
	virtual void SetUp() override {
		base_path = Poco::Path(Poco::Path::current()).toString();
		componentManager->startComponent("iocontroller");
	}
	
	virtual void TearDown() override {
		componentManager->stopComponent("iocontroller");
	}

	virtual void GoodCases() override {

		
		auto evt1 = createEvent("io::makeDir");
		evt1->payload["dir"] = base_path + "/component_test/test_dir";
		auto result1 = publish_sync(std::move(evt1));
		EXPECT_TRUE(static_cast<bool>(result1->payload["success"]));
        
		
		auto evt2 = createEvent("io::writeFile");
		evt2->payload["filename"] = base_path + "/component_test/test.txt";
		evt2->payload["content"] = "foobar";	
		auto result2 = publish_sync(std::move(evt2));	
		int bytesWritten = result2->payload["bytesWritten"];
		EXPECT_EQ(6, bytesWritten);

		auto evt3 = createEvent("io::readFile");
		evt3->payload["filename"] = base_path +  "/component_test/test.txt";
		auto result3 = publish_sync(std::move(evt3));
		std::string content = result3->payload["content"];
		EXPECT_EQ("foobar", content);

       
	}

	virtual void BadCases() override {

	}

	virtual void EdgeCases() override {

	}

};

TEST_F(IOControllerComponentTest,GoodCases){
	GoodCases();
}

TEST_F(IOControllerComponentTest,BadCases){
	BadCases();
}

TEST_F(IOControllerComponentTest,EdgeCases){
	EdgeCases();
}
