#include "gtest/gtest.h"
#include "susi/iocontroller/IOController.h"
#include "susi/util/TestHelper.h"


class IOControllerTest : public ::testing::Test {
	public:
		std::string current_path;
		IOControllerTest(){};
		virtual ~IOControllerTest(){}
	protected:
		Susi::IOController controller;
		std::string data = "foobar";
		virtual void SetUp() override {
			current_path = Poco::Path::current();
			controller.makeDir(current_path + "/IOTESTS/CHECKDIR/");
		}
		virtual void TearDown() override {
			controller.deletePath(current_path +"/IOTESTS/");
		}
};


TEST_F(IOControllerTest,WriteFileInNoneExistentDirectory){
	EXPECT_THROW ({
		controller.writeFile(current_path + "/IOTESTS/NO_EXISTENT_DIR/foobar.dat",data);
	}, std::runtime_error);
}

TEST_F(IOControllerTest,WriteFile){
	std::size_t bs = controller.writeFile(current_path + "/IOTESTS/foobar_write.dat",data);

	EXPECT_EQ(size_t{6},bs);

	std::size_t w_size = 1024*1024*5;
	data = Susi::Util::__createRandomString(w_size);
	bs   = controller.writeFile(current_path + "/IOTESTS/foobar_write_large.dat",data);

	EXPECT_EQ(w_size,bs);
}

TEST_F(IOControllerTest,ReadFile){
	// create File
	controller.writeFile(current_path +"/IOTESTS/foobar_read.dat",data);

	std::string result = controller.readFile(current_path + "/IOTESTS/foobar_read.dat");
	EXPECT_EQ(data,result);
}

TEST_F(IOControllerTest,ReadFileWithNoReadPermission){
	// create File
	std::string path = current_path +"/IOTESTS/foobar_read.dat";
	controller.writeFile(current_path +"/IOTESTS/foobar_read.dat",data);

	chmod((char*)path.c_str(), 0222);

	EXPECT_THROW ({
		std::string result = controller.readFile(current_path + "/IOTESTS/foobar_read.dat");
	}, std::runtime_error);

	chmod((char*)path.c_str(), 0777);
}

TEST_F(IOControllerTest,ReadFileInNoneExistentDir){
	EXPECT_THROW ({
		controller.readFile(current_path +"/IOTESTS/NO_EXISTENT_DIR/foobar.dat");
	}, std::runtime_error);
}

TEST_F(IOControllerTest,ReadFileInNoneExistentFile){
	EXPECT_THROW ({
		controller.readFile(current_path +"/IOTESTS/foobar_dont_exists.dat");
	}, std::runtime_error);
}

TEST_F(IOControllerTest,ReadFileInNoneExistentFile2){
	EXPECT_THROW ({
		controller.readFile(current_path + "/foobar_dont_exists.dat");
	}, std::runtime_error);
}

TEST_F(IOControllerTest,DeletePath_DIR){
	// create File
	controller.makeDir(current_path + "/IOTESTS/DIR_TO_BE_DELETED/");

	// delete first time
	bool result = false;
	result = controller.deletePath(current_path + "/IOTESTS/DIR_TO_BE_DELETED/");
	EXPECT_TRUE(result);

	// delete second time
	result = controller.deletePath(current_path + "/IOTESTS/DIR_TO_BE_DELETED/");
	EXPECT_FALSE(result);
}

TEST_F(IOControllerTest,DeletePath_DEVICE){	
	EXPECT_THROW ({
		controller.deletePath("/dev/null");
	}, std::exception);
}

TEST_F(IOControllerTest,DeletePath_FILE){
	// create File
	controller.writeFile(current_path + "/IOTESTS/foobar_to_be_deleted.dat",data);

	// delete first time
	bool result = false;
	result = controller.deletePath(current_path + "/IOTESTS/foobar_to_be_deleted.dat");
	EXPECT_TRUE(result);

	// delete second time
	result = controller.deletePath(current_path +"/IOTESTS/foobar_to_be_deleted.dat");
	EXPECT_FALSE(result);
}

TEST_F(IOControllerTest,MakeDir){
	bool result = controller.makeDir(current_path + "/IOTESTS/TEST_MAKE_DIR");
	EXPECT_TRUE(result);
}

TEST_F(IOControllerTest,MakeDirNoPermissions){
	bool result = false;
	std::string path = current_path +"/IOTESTS/TEST_MAKE_DIR";

	result = controller.makeDir(path);
	EXPECT_TRUE(result);

	chmod((char*)path.c_str(), 0111);

	EXPECT_THROW ({
		result = controller.makeDir(path+ "/SUB");
	}, std::exception);

	chmod((char*)path.c_str(), 0777);	

	
}

TEST_F(IOControllerTest, SetAndGetExecutable){
	controller.writeFile(current_path + "/IOTESTS/script.sh","foobar");

	bool result = controller.getExecutable(current_path +"/IOTESTS/script.sh");
	EXPECT_FALSE(result);

	controller.setExecutable(current_path + "/IOTESTS/script.sh", true);
	result = controller.getExecutable(current_path + "/IOTESTS/script.sh");
	EXPECT_TRUE(result);

	controller.setExecutable(current_path + "/IOTESTS/script.sh", false);
	result = controller.getExecutable(current_path + "/IOTESTS/script.sh");
	EXPECT_FALSE(result);
}

TEST_F(IOControllerTest,CheckDir){

	bool result = controller.checkDir(Poco::Path(current_path + "/IOTESTS/NO_CHECKDIR"));

	EXPECT_FALSE(result);

	result = controller.checkDir(Poco::Path(current_path + "/IOTESTS/CHECKDIR"));

	EXPECT_TRUE(result);
}

TEST_F(IOControllerTest,CheckFile){

	bool result = controller.checkFile(Poco::Path(current_path + "/IOTESTS/none_exitent_file.dat"));

	EXPECT_FALSE(result);

	controller.writeFile(current_path + "/IOTESTS/exitent_file.dat",data);
	result = controller.checkFile(Poco::Path(current_path + "/IOTESTS/exitent_file.dat"));

	EXPECT_TRUE(result);
}

TEST_F(IOControllerTest,MovePath_DIR_TO_DIR){
	bool result = false;

	// check init from SetUp
	result = controller.checkDir(Poco::Path(current_path + "/IOTESTS/CHECKDIR"));
	EXPECT_TRUE(result);

	// move first time
	result = controller.movePath(current_path + "/IOTESTS/CHECKDIR", current_path + "/CHECKDIR");
	EXPECT_TRUE(result);

	// move secound time
	EXPECT_THROW ({
		controller.movePath(current_path + "/IOTESTS/CHECKDIR", current_path + "/CHECKDIR");
	}, std::runtime_error);

	// check old location
	result = controller.checkDir(Poco::Path(current_path + "/IOTESTS/CHECKDIR"));
	EXPECT_FALSE(result);

	// move back for TearDown
	result = controller.movePath(current_path + "/CHECKDIR", current_path + "/IOTESTS/CHECKDIR");
	EXPECT_TRUE(result);

	// check new location
	result = controller.checkDir(Poco::Path(current_path + "/CHECKDIR"));
	EXPECT_FALSE(result);
}

TEST_F(IOControllerTest,MovePath_FILE_TO_DIR){
	bool result = false;

	// check init
	controller.writeFile(current_path + "/IOTESTS/move_file.dat",data);
	result = controller.checkFile(Poco::Path(current_path + "/IOTESTS/move_file.dat"));
	EXPECT_TRUE(result);

	// move first time
	result = controller.movePath(current_path + "/IOTESTS/move_file.dat", current_path + "/IOTESTS/CHECKDIR");
	EXPECT_TRUE(result);

	// move secound time
	EXPECT_THROW ({
		controller.movePath(current_path + "/IOTESTS/move_file.dat", current_path + "/IOTESTS/CHECKDIR");
	}, std::runtime_error);

	// check old location
	result = controller.checkFile(Poco::Path(current_path + "/IOTESTS/move_file.dat"));
	EXPECT_FALSE(result);

	// check new location
	result = controller.checkFile(Poco::Path(current_path + "/IOTESTS/CHECKDIR/move_file.dat"));
	EXPECT_TRUE(result);
}

TEST_F(IOControllerTest,MovePath_FILE_TO_FILE){
	bool result = false;

	// check init
	controller.writeFile(current_path + "/IOTESTS/move_file.dat",data);
	result = controller.checkFile(Poco::Path(current_path + "/IOTESTS/move_file.dat"));
	EXPECT_TRUE(result);

	// move first time
	result = controller.movePath(current_path + "/IOTESTS/move_file.dat", current_path + "/IOTESTS/moved_file.dat");
	EXPECT_TRUE(result);

	// move secound time
	EXPECT_THROW ({
		controller.movePath(current_path + "/IOTESTS/move_file.dat", current_path + "/IOTESTS/moved_file.dat");
	}, std::runtime_error);

	// check old location
	result = controller.checkFile(Poco::Path(current_path + "/IOTESTS/move_file.dat"));
	EXPECT_FALSE(result);

	// check new location
	result = controller.checkFile(Poco::Path(current_path + "/IOTESTS/moved_file.dat"));
	EXPECT_TRUE(result);
}

TEST_F(IOControllerTest,CopyPath_DIR_TO_DIR){
	bool result = false;

	// check init from SetUp
	result = controller.checkDir(Poco::Path(current_path + "/IOTESTS/CHECKDIR"));
	EXPECT_TRUE(result);

	// copy
	result = controller.copyPath(current_path + "/IOTESTS/CHECKDIR", current_path + "/CHECKDIR");
	EXPECT_TRUE(result);

	// check old location
	result = controller.checkDir(Poco::Path(current_path + "/IOTESTS/CHECKDIR"));
	EXPECT_TRUE(result);

	// check new location
	result = controller.checkDir(Poco::Path(current_path + "/CHECKDIR"));
	EXPECT_TRUE(result);

	// clean up
	controller.deletePath(current_path + "/CHECKDIR");
}

TEST_F(IOControllerTest,CopyPath_FILE_TO_DIR){
	bool result = false;

	// init
	controller.writeFile(current_path + "/IOTESTS/copy_file.dat",data);
	result = controller.checkDir(Poco::Path(current_path + "/IOTESTS/CHECKDIR"));
	EXPECT_TRUE(result);

	// copy
	result = controller.copyPath(current_path + "/IOTESTS/copy_file.dat", current_path + "/IOTESTS/CHECKDIR");
	EXPECT_TRUE(result);

	// check old location
	result = controller.checkFile(Poco::Path(current_path + "/IOTESTS/copy_file.dat"));
	EXPECT_TRUE(result);

	// check new location
	result = controller.checkFile(Poco::Path(current_path + "/IOTESTS/CHECKDIR/copy_file.dat"));
	EXPECT_TRUE(result);
}

TEST_F(IOControllerTest,CopyPath_FILE_TO_FILE){
	bool result = false;

	// init
	controller.writeFile(current_path + "/IOTESTS/copy_file.dat",data);
	result = controller.checkFile(Poco::Path(current_path + "/IOTESTS/copied_file.dat"));
	EXPECT_FALSE(result);

	// copy
	result = controller.copyPath(current_path + "/IOTESTS/copy_file.dat", current_path+ "/IOTESTS/copied_file.dat");
	EXPECT_TRUE(result);

	// check old location
	result = controller.checkFile(Poco::Path(current_path + "/IOTESTS/copy_file.dat"));
	EXPECT_TRUE(result);

	// check new location
	result = controller.checkFile(Poco::Path(current_path + "/IOTESTS/copied_file.dat"));
	EXPECT_TRUE(result);
}

TEST_F(IOControllerTest,CheckFileExtension){
	
	bool result = false;

	// wrong extension
	result = controller.checkFileExtension("file.dat", "txt");
	EXPECT_FALSE(result);

	// file too short
	result = controller.checkFileExtension("dat", "dat");
	EXPECT_FALSE(result);

}
