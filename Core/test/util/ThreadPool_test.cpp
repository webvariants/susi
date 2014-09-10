#include "gtest/gtest.h"
#include "util/ThreadPool.h"
#include <condition_variable>

class ThreadPoolTest : public ::testing::Test {
public:
	ThreadPoolTest() : pool{4,32} {};
protected:
	Susi::Util::ThreadPool pool;
};

TEST_F(ThreadPoolTest,Basic){
	std::mutex m1,m2,m3,m4;
	bool c1=false, c2=false, c3=false, c4=false;
	std::condition_variable cv1,cv2,cv3,cv4;
	pool.add([&c1,&cv1](){
		c1 = true;
		cv1.notify_one();
	});
	pool.add([&c2,&cv2](){
		c2 = true;
		cv2.notify_one();
	});
	pool.add([&c3,&cv3](){
		c3 = true;
		cv3.notify_one();
	});
	pool.add([&c4,&cv4](){
		c4 = true;
		cv4.notify_one();
	});
	{
		std::unique_lock<std::mutex> lk(m1);
		cv1.wait_for(lk,
			std::chrono::duration<int,std::milli>{250},
			[&c1](){return c1;});
		EXPECT_TRUE(c1);
	}
	{
		std::unique_lock<std::mutex> lk(m2);
		cv2.wait_for(lk,
			std::chrono::duration<int,std::milli>{250},
			[&c2](){return c2;});
		EXPECT_TRUE(c2);
	}
	{
		std::unique_lock<std::mutex> lk(m3);
		cv3.wait_for(lk,
			std::chrono::duration<int,std::milli>{250},
			[&c3](){return c3;});
		EXPECT_TRUE(c3);
	}
	{
		std::unique_lock<std::mutex> lk(m4);
		cv4.wait_for(lk,
			std::chrono::duration<int,std::milli>{250},
			[&c4](){return c4;});
		EXPECT_TRUE(c4);
	}
}