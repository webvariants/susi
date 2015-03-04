#include "gtest/gtest.h"
#include "susi/autodiscovery/AutoDiscoveryManager.h"
#include <mutex>
#include <vector>
#include <utility>

#include <Poco/Thread.h>

class AutoDiscoveryManagerTest : public ::testing::Test {
protected:	
	std::mutex mutex;
	std::vector<std::pair<std::string,std::string>> list;
	
	class TestManager : public Susi::Autodiscovery::Manager {
	public:
		TestManager(std::string addr1,std::string addr2,AutoDiscoveryManagerTest *test) : Susi::Autodiscovery::Manager{addr1,addr2}, _test{test} {}
	protected:
		AutoDiscoveryManagerTest *_test;
		virtual void onNewHost(std::string & addr) override {
			std::lock_guard<std::mutex> lock{_test->mutex};
			_test->list.push_back(std::pair<std::string,std::string>{_ownAddr,addr});
		}
	};

	virtual void TearDown() override {
		list.clear();
	}

};

TEST_F(AutoDiscoveryManagerTest, Basic) {
	TestManager manager1{"239.255.255.250:12345","Host1", this};
	TestManager manager2{"239.255.255.250:12345","Host2", this};
	TestManager manager3{"239.255.255.250:12345","Host3", this};
	manager1.start();
	manager2.start();
	manager3.start();
	//std::this_thread::sleep_for(std::chrono::milliseconds{250});
	Poco::Thread::sleep(250);
	manager1.stop();
	manager2.stop();
	manager3.stop();
	std::vector<std::pair<std::string,std::string>> expected = {
		{"Host1","Host2"},
		{"Host1","Host3"},
		{"Host2","Host1"},
		{"Host2","Host3"},
		{"Host3","Host1"},
		{"Host3","Host2"}
	};
	for(auto & pair : expected){
		for(auto it = list.begin(); it != list.end(); it++){
			if(pair.first == (*it).first && pair.second == (*it).second){
				list.erase(it);
				break;
			}
		}
	}
	EXPECT_TRUE(list.empty());
}