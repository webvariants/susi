#include "gtest/gtest.h"
#include "util/Channel.h"
#include <thread>
#include <functional>
#include <atomic>

class ChannelTest : public ::testing::Test {
protected:
	class Producer {
	protected:
		Susi::Util::Channel<int> * _ch;
		std::thread _thread;
	public:
		Producer(Susi::Util::Channel<int> * ch, size_t count, int value) : _ch{ch}{
			_thread = std::thread{[this,count,value](){
				for(size_t i=0;i<count;++i){
					_ch->put(value);
				}
				_ch->close();
			}};
		}
		void join(){
			if(_thread.joinable())_thread.join();
		}
		~Producer(){
			join();
		}
	};
	class Consumer {
	protected:
		Susi::Util::Channel<int> * _ch;
		std::function<void(int)> _predicate;
		std::thread _thread;
	public:
		Consumer(Susi::Util::Channel<int> * ch, std::function<void(int)> predicate = 0) : 
			_ch{ch} , 
			_predicate{predicate} {
			_thread = std::thread{
				[this](){
					try{
						while(true){
							int val = _ch->get();
							if(_predicate)_predicate(val);
						}
					}
					catch(const std::exception & e){
						return;
					}
				}
			};
		}
		void join(){
			if(_thread.joinable())_thread.join();
		}
		~Consumer(){
			join();
		}
	};
};

TEST_F(ChannelTest,SimpleUnbuffered){
	int count = 0;
	Susi::Util::Channel<int> ch;
	Producer p{&ch,1,1};
	Consumer c{&ch,[&count](int val){EXPECT_EQ(1,val);count++;}};
	p.join();
	c.join();
	EXPECT_EQ(1,count);
}

TEST_F(ChannelTest,SimpleUnbufferedMulti){
	int count = 0;
	Susi::Util::Channel<int> ch;
	Producer p{&ch,100,1};
	Consumer c{&ch,[&count](int val){EXPECT_EQ(1,val);count++;}};
	p.join();
	c.join();
	EXPECT_EQ(100,count);
}

TEST_F(ChannelTest,SimpleBuffered){
	int count = 0;
	Susi::Util::Channel<int> ch{10};
	Producer p{&ch,1,1};
	Consumer c{&ch,[&count](int val){EXPECT_EQ(1,val);count++;}};
	p.join();
	c.join();
	EXPECT_EQ(1,count);
}

TEST_F(ChannelTest,SimpleBufferedMulti){
	int count = 0;
	Susi::Util::Channel<int> ch{10};
	Producer p{&ch,100,1};
	Consumer c{&ch,[&count](int val){EXPECT_EQ(1,val);count++;}};
	p.join();
	c.join();
	EXPECT_EQ(100,count);
}

TEST_F(ChannelTest,MultiProducer){
	int count = 0;
	Susi::Util::Channel<int> ch{10};
	auto prodFunc = [&ch](){
		for(int i=0;i<100;++i){
			ch.put(1);
		}
	};
	auto consumerFunc = [&ch,&count](){
		for(int i=0;i<300;i++){
			EXPECT_EQ(1,ch.get());
			count++;
		}
	};
	std::thread t1{prodFunc};
	std::thread t2{prodFunc};
	std::thread t3{prodFunc};
	std::thread t4{consumerFunc};
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	EXPECT_EQ(300,count);	
}

TEST_F(ChannelTest,MultiConsumer){
	std::atomic<int> count;
	count.store(0);
	Susi::Util::Channel<int> ch{10};
	auto consumerFunc = [&ch,&count](){
		EXPECT_THROW({
			while(true){
				ch.get();
				count++;
			}	
		},Susi::Util::ChannelClosedException);
	};
	std::thread t1{consumerFunc};
	std::thread t2{consumerFunc};
	std::thread t3{consumerFunc};
	for(int i=0;i<300;++i){
		ch.put(1);
	}
	ch.close();
	t1.join();
	t2.join();
	t3.join();
	EXPECT_EQ(300,count);	
}

TEST_F(ChannelTest,CloseTest){
	Susi::Util::Channel<int> ch{10};
	auto consumer = [&ch](){
		EXPECT_THROW({
			while(true){
				ch.get();
			}
		},Susi::Util::ChannelClosedException);	
	};
	std::thread t1{consumer};
	ch.close();
	t1.join();
}