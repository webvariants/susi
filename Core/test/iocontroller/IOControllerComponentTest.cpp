#include "susi/util/ComponentTest.h"

class IOControllerComponentTest : public ComponentTest {

public:
	std::string base_path;
	Susi::IOController io;
protected:
	
	virtual void SetUp() override {
		base_path = Poco::Path(Poco::Path::current()).toString() + "/iocontroller_test";		
		io.makeDir(base_path);
		componentManager->startComponent("iocontroller");
	}
	
	virtual void TearDown() override {
		io.deletePath(base_path);
		componentManager->stopComponent("iocontroller");
	}

	virtual void GoodCases() override {

		
		auto evt1 = createEvent("io::makeDir");
		evt1->payload["dir"] = base_path + "/test_dir";
		auto result1 = publish_sync(std::move(evt1));
		EXPECT_TRUE(static_cast<bool>(result1->payload["success"]));
        		
		auto evt2 = createEvent("io::writeFile");
		evt2->payload["filename"] = base_path + "/test_dir/test.txt";
		evt2->payload["content"] = "foobar";	
		auto result2 = publish_sync(std::move(evt2));	
		int bytesWritten = result2->payload["bytesWritten"];
		EXPECT_EQ(6, bytesWritten);

		auto evt3 = createEvent("io::readFile");
		evt3->payload["filename"] = base_path +  "/test_dir/test.txt";
		auto result3 = publish_sync(std::move(evt3));
		std::string content = result3->payload["content"];
		EXPECT_EQ("foobar", content);

		auto evt4 = createEvent("io::copyPath");
		evt4->payload["source_path"] = base_path +  "/test_dir";
		evt4->payload["dest_path"] = base_path +  "/test_dir2";
		auto result4 = publish_sync(std::move(evt4));
		EXPECT_TRUE(static_cast<bool>(result4->payload["success"]));

		auto evt5 = createEvent("io::readFile");
		evt5->payload["filename"] = base_path +  "/test_dir2/test.txt";
		auto result5 = publish_sync(std::move(evt5));
		std::string content2 = result5->payload["content"];		
		EXPECT_EQ("foobar", content2);

		auto evt6 = createEvent("io::movePath");
		evt6->payload["source_path"] = base_path +  "/test_dir2";
		evt6->payload["dest_path"] = base_path +  "/test_dir3";
		auto result6 = publish_sync(std::move(evt6));
		EXPECT_TRUE(static_cast<bool>(result6->payload["success"]));

		auto evt7 = createEvent("io::readFile");
		evt7->payload["filename"] = base_path +  "/test_dir3/test.txt";
		auto result7 = publish_sync(std::move(evt7));
		std::string content3 = result7->payload["content"];		
		EXPECT_EQ("foobar", content2);

		auto evt8 = createEvent("io::deletePath");
		evt8->payload["path"] = base_path +  "/test_dir3";
		auto result8 = publish_sync(std::move(evt8));
		EXPECT_TRUE(static_cast<bool>(result8->payload["success"]));

		auto evt9 = createEvent("io::setExecutable");
		evt9->payload["path"] = base_path +  "/test_dir/test.txt";
		evt9->payload["isExecutable"] = true;
		auto result9 = publish_sync(std::move(evt9));
		EXPECT_TRUE(static_cast<bool>(result9->payload["success"]));

		auto evt10 = createEvent("io::getExecutable");
		evt10->payload["path"] = base_path +  "/test_dir/test.txt";
		auto result10 = publish_sync(std::move(evt10));
		EXPECT_TRUE(static_cast<bool>(result10->payload["success"]));

		auto evt11 = createEvent("io::setExecutable");
		evt11->payload["path"] = base_path +  "/test_dir/test.txt";
		evt11->payload["isExecutable"] = false;
		auto result11 = publish_sync(std::move(evt11));
		EXPECT_TRUE(static_cast<bool>(result11->payload["success"]));

		auto evt12 = createEvent("io::getExecutable");
		evt12->payload["path"] = base_path +  "/test_dir/test.txt";
		auto result12 = publish_sync(std::move(evt12));
		EXPECT_FALSE(static_cast<bool>(result12->payload["success"]));

	}

	virtual void BadCases() override {

		auto evt = createEvent("io::writeFile");
		evt->payload["filename"] = base_path + "/test_dir/test.txt";
		evt->payload["content"] = "foobar";	
		auto result = publish_sync(std::move(evt));	

		//std::cout<<"EDGE CASE:"<<result->toAny().toJSONString()<<std::endl;
		EXPECT_FALSE(static_cast<bool>(result->payload["success"]));
		EXPECT_TRUE(hasErrorHeader(result));

		auto evt2 = createEvent("io::readFile");
		evt2->payload["filename"] = base_path +  "/test_dir/test.txt";
		auto result2 = publish_sync(std::move(evt2));
		EXPECT_FALSE(static_cast<bool>(result2->payload["success"]));
		EXPECT_TRUE(hasErrorHeader(result2));

		auto evt3 = createEvent("io::movePath");
		evt3->payload["source_path"] = base_path +  "/test_dir";
		evt3->payload["dest_path"] = base_path +  "/test_dir2";
		auto result3 = publish_sync(std::move(evt3));
		EXPECT_FALSE(static_cast<bool>(result3->payload["success"]));
		EXPECT_TRUE(hasErrorHeader(result3));

		auto evt4 = createEvent("io::deletePath");
		evt4->payload["path"] = base_path +  "/test_dir";
		auto result4 = publish_sync(std::move(evt4));
		EXPECT_FALSE(static_cast<bool>(result4->payload["success"]));		

		auto evt5 = createEvent("io::copyPath");
		evt5->payload["source_path"] = base_path +  "/test_dir";
		evt5->payload["dest_path"] = base_path +  "/test_dir2";
		auto result5 = publish_sync(std::move(evt5));
		EXPECT_FALSE(static_cast<bool>(result5->payload["success"]));
		EXPECT_TRUE(hasErrorHeader(result5));


		auto evt6 = createEvent("io::setExecutable");
		evt6->payload["path"] = base_path +  "/test_dir/test.txt";
		evt6->payload["isExecutable"] = true;
		auto result6 = publish_sync(std::move(evt6));
		EXPECT_FALSE(static_cast<bool>(result6->payload["success"]));
		EXPECT_TRUE(hasErrorHeader(result6));

		auto evt7 = createEvent("io::getExecutable");
		evt7->payload["path"] = base_path +  "/test_dir/test.txt";
		auto result7 = publish_sync(std::move(evt7));
		EXPECT_FALSE(static_cast<bool>(result7->payload["success"]));
		EXPECT_TRUE(hasErrorHeader(result7));

	}

	virtual void EdgeCases() override {

		// test with missing params

		auto evt = createEvent("io::writeFile");
		auto result = publish_sync(std::move(evt));	

		//std::cout<<"EDGE CASE:"<<result->toAny().toJSONString()<<std::endl;
		EXPECT_FALSE(static_cast<bool>(result->payload["success"]));
		EXPECT_TRUE(hasErrorHeader(result));

		auto evt2 = createEvent("io::readFile");
		auto result2 = publish_sync(std::move(evt2));
		EXPECT_FALSE(static_cast<bool>(result2->payload["success"]));
		EXPECT_TRUE(hasErrorHeader(result2));

		auto evt3 = createEvent("io::movePath");
		auto result3 = publish_sync(std::move(evt3));
		EXPECT_FALSE(static_cast<bool>(result3->payload["success"]));
		EXPECT_TRUE(hasErrorHeader(result3));

		auto evt4 = createEvent("io::deletePath");
		auto result4 = publish_sync(std::move(evt4));
		EXPECT_FALSE(static_cast<bool>(result4->payload["success"]));		
		EXPECT_TRUE(hasErrorHeader(result4));

		auto evt5 = createEvent("io::copyPath");		
		auto result5 = publish_sync(std::move(evt5));
		EXPECT_FALSE(static_cast<bool>(result5->payload["success"]));
		EXPECT_TRUE(hasErrorHeader(result5));


		auto evt6 = createEvent("io::setExecutable");		
		auto result6 = publish_sync(std::move(evt6));
		EXPECT_FALSE(static_cast<bool>(result6->payload["success"]));
		EXPECT_TRUE(hasErrorHeader(result6));

		auto evt7 = createEvent("io::getExecutable");
		auto result7 = publish_sync(std::move(evt7));
		EXPECT_FALSE(static_cast<bool>(result7->payload["success"]));
		EXPECT_TRUE(hasErrorHeader(result7));
	

		auto evt8 = createEvent("io::makeDir");
		chmod((char*)base_path.c_str(), 0111);
		evt8->payload["dir"] = base_path + "/test_dir";
		auto result8 = publish_sync(std::move(evt8));		
		EXPECT_FALSE(static_cast<bool>(result8->payload["success"]));
		EXPECT_TRUE(hasErrorHeader(result8));
		chmod((char*)base_path.c_str(), 0777);		
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
