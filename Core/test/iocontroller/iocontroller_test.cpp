#include "gtest/gtest.h"
#include "iocontroller/IOController.h"
#include "util/TestHelper.h"


class IOControllerTest : public ::testing::Test {
	public:
		IOControllerTest(){};
		virtual ~IOControllerTest(){}
	protected:
		Susi::IOController controller;
		std::string data = "foobar";
		virtual void SetUp() override {
			controller.makeDir("./IOTESTS/CHECKDIR/");
		}
		virtual void TearDown() override {
			controller.deletePath("./IOTESTS/");
		}
};

TEST_F(IOControllerTest,Construct){
	std::string current_path = Poco::Path::current();
	EXPECT_EQ(current_path,controller.base_path.toString());

	controller = Susi::IOController("/base");

	EXPECT_EQ("/base",controller.base_path.toString());
}

TEST_F(IOControllerTest,WriteFileInNoneExistentDirectory){
	EXPECT_THROW ({
		controller.writeFile("./IOTESTS/NO_EXISTENT_DIR/foobar.dat",data);
	}, std::runtime_error);
}

TEST_F(IOControllerTest,WriteFile){
	std::size_t bs = controller.writeFile("./IOTESTS/foobar_write.dat",data);

	EXPECT_EQ(size_t{6},bs);

	std::size_t w_size = 1024*1024*5;
	data = Susi::Util::__createRandomString(w_size);
	bs   = controller.writeFile("./IOTESTS/foobar_write_large.dat",data);

	EXPECT_EQ(w_size,bs);
}

TEST_F(IOControllerTest,ReadFile){
	// create File
	controller.writeFile("./IOTESTS/foobar_read.dat",data);

	std::string result = controller.readFile("./IOTESTS/foobar_read.dat");
	EXPECT_EQ(data,result);
}

TEST_F(IOControllerTest,ReadFileInNoneExistentDir){
	EXPECT_THROW ({
		controller.readFile("./IOTESTS/NO_EXISTENT_DIR/foobar.dat");
	}, std::runtime_error);
}

TEST_F(IOControllerTest,ReadFileInNoneExistentFile){
	EXPECT_THROW ({
		controller.readFile("./IOTESTS/foobar_dont_exists.dat");
	}, std::runtime_error);
}

TEST_F(IOControllerTest,ReadFileInNoneExistentFile2){
	EXPECT_THROW ({
		controller.readFile("./foobar_dont_exists.dat");
	}, std::runtime_error);
}

TEST_F(IOControllerTest,DeletePath_DIR){
	// create File
	controller.makeDir("./IOTESTS/DIR_TO_BE_DELETED/");

	// delete first time
	bool result = false;
	result = controller.deletePath("./IOTESTS/DIR_TO_BE_DELETED/");
	EXPECT_TRUE(result);

	// delete second time
	result = controller.deletePath("./IOTESTS/DIR_TO_BE_DELETED/");
	EXPECT_FALSE(result);
}

TEST_F(IOControllerTest,DeletePath_FILE){
	// create File
	controller.writeFile("./IOTESTS/foobar_to_be_deleted.dat",data);

	// delete first time
	bool result = false;
	result = controller.deletePath("./IOTESTS/foobar_to_be_deleted.dat");
	EXPECT_TRUE(result);

	// delete second time
	result = controller.deletePath("./IOTESTS/foobar_to_be_deleted.dat");
	EXPECT_FALSE(result);
}

TEST_F(IOControllerTest,MakeDir){
	bool result = controller.makeDir("./IOTESTS/TEST_MAKE_DIR");
	EXPECT_TRUE(result);
}

TEST_F(IOControllerTest, SetAndGetExecutable){
	controller.writeFile("./IOTESTS/script.sh","foobar");

	bool result = controller.getExecutable("./IOTESTS/script.sh");
	EXPECT_FALSE(result);

	controller.setExecutable("./IOTESTS/script.sh", true);
	result = controller.getExecutable("./IOTESTS/script.sh");
	EXPECT_TRUE(result);

	controller.setExecutable("./IOTESTS/script.sh", false);
	result = controller.getExecutable("./IOTESTS/script.sh");
	EXPECT_FALSE(result);
}

TEST_F(IOControllerTest,CheckDir){

	bool result = controller.checkDir(Poco::Path("./IOTESTS/NO_CHECKDIR"));

	EXPECT_FALSE(result);

	result = controller.checkDir(Poco::Path("./IOTESTS/CHECKDIR"));

	EXPECT_TRUE(result);
}

TEST_F(IOControllerTest,CheckFile){

	bool result = controller.checkFile(Poco::Path("./IOTESTS/none_exitent_file.dat"));

	EXPECT_FALSE(result);

	controller.writeFile("./IOTESTS/exitent_file.dat",data);
	result = controller.checkFile(Poco::Path("./IOTESTS/exitent_file.dat"));

	EXPECT_TRUE(result);
}

TEST_F(IOControllerTest,MovePath_DIR_TO_DIR){
	bool result = false;

	// check init from SetUp
	result = controller.checkDir(Poco::Path("./IOTESTS/CHECKDIR"));
	EXPECT_TRUE(result);

	// move first time
	result = controller.movePath("./IOTESTS/CHECKDIR", "./CHECKDIR");
	EXPECT_TRUE(result);

	// move secound time
	EXPECT_THROW ({
		controller.movePath("./IOTESTS/CHECKDIR", "./CHECKDIR");
	}, std::runtime_error);

	// check old location
	result = controller.checkDir(Poco::Path("./IOTESTS/CHECKDIR"));
	EXPECT_FALSE(result);

	// move back for TearDown
	result = controller.movePath("./CHECKDIR", "./IOTESTS/CHECKDIR");
	EXPECT_TRUE(result);

	// check new location
	result = controller.checkDir(Poco::Path("./CHECKDIR"));
	EXPECT_FALSE(result);
}

TEST_F(IOControllerTest,MovePath_FILE_TO_DIR){
	bool result = false;

	// check init
	controller.writeFile("./IOTESTS/move_file.dat",data);
	result = controller.checkFile(Poco::Path("./IOTESTS/move_file.dat"));
	EXPECT_TRUE(result);

	// move first time
	result = controller.movePath("./IOTESTS/move_file.dat", "./IOTESTS/CHECKDIR");
	EXPECT_TRUE(result);

	// move secound time
	EXPECT_THROW ({
		controller.movePath("./IOTESTS/move_file.dat", "./IOTESTS/CHECKDIR");
	}, std::runtime_error);

	// check old location
	result = controller.checkFile(Poco::Path("./IOTESTS/move_file.dat"));
	EXPECT_FALSE(result);

	// check new location
	result = controller.checkFile(Poco::Path("./IOTESTS/CHECKDIR/move_file.dat"));
	EXPECT_TRUE(result);
}

TEST_F(IOControllerTest,MovePath_FILE_TO_FILE){
	bool result = false;

	// check init
	controller.writeFile("./IOTESTS/move_file.dat",data);
	result = controller.checkFile(Poco::Path("./IOTESTS/move_file.dat"));
	EXPECT_TRUE(result);

	// move first time
	result = controller.movePath("./IOTESTS/move_file.dat", "./IOTESTS/moved_file.dat");
	EXPECT_TRUE(result);

	// move secound time
	EXPECT_THROW ({
		controller.movePath("./IOTESTS/move_file.dat", "./IOTESTS/moved_file.dat");
	}, std::runtime_error);

	// check old location
	result = controller.checkFile(Poco::Path("./IOTESTS/move_file.dat"));
	EXPECT_FALSE(result);

	// check new location
	result = controller.checkFile(Poco::Path("./IOTESTS/moved_file.dat"));
	EXPECT_TRUE(result);
}

TEST_F(IOControllerTest,CopyPath_DIR_TO_DIR){
	bool result = false;

	// check init from SetUp
	result = controller.checkDir(Poco::Path("./IOTESTS/CHECKDIR"));
	EXPECT_TRUE(result);

	// copy
	result = controller.copyPath("./IOTESTS/CHECKDIR", "./CHECKDIR");
	EXPECT_TRUE(result);

	// check old location
	result = controller.checkDir(Poco::Path("./IOTESTS/CHECKDIR"));
	EXPECT_TRUE(result);

	// check new location
	result = controller.checkDir(Poco::Path("./CHECKDIR"));
	EXPECT_TRUE(result);

	// clean up
	controller.deletePath("./CHECKDIR");
}

TEST_F(IOControllerTest,CopyPath_FILE_TO_DIR){
	bool result = false;

	// init
	controller.writeFile("./IOTESTS/copy_file.dat",data);
	result = controller.checkDir(Poco::Path("./IOTESTS/CHECKDIR"));
	EXPECT_TRUE(result);

	// copy
	result = controller.copyPath("./IOTESTS/copy_file.dat", "./IOTESTS/CHECKDIR");
	EXPECT_TRUE(result);

	// check old location
	result = controller.checkFile(Poco::Path("./IOTESTS/copy_file.dat"));
	EXPECT_TRUE(result);

	// check new location
	result = controller.checkFile(Poco::Path("./IOTESTS/CHECKDIR/copy_file.dat"));
	EXPECT_TRUE(result);
}

TEST_F(IOControllerTest,CopyPath_FILE_TO_FILE){
	bool result = false;

	// init
	controller.writeFile("./IOTESTS/copy_file.dat",data);
	result = controller.checkFile(Poco::Path("./IOTESTS/copied_file.dat"));
	EXPECT_FALSE(result);

	// copy
	result = controller.copyPath("./IOTESTS/copy_file.dat", "./IOTESTS/copied_file.dat");
	EXPECT_TRUE(result);

	// check old location
	result = controller.checkFile(Poco::Path("./IOTESTS/copy_file.dat"));
	EXPECT_TRUE(result);

	// check new location
	result = controller.checkFile(Poco::Path("./IOTESTS/copied_file.dat"));
	EXPECT_TRUE(result);
}
