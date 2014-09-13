#include "gtest/gtest.h"
#include "autodiscovery/AutoDiscoveryManager.h"

TEST(AutoDiscoveryManager, Basic) {
	class TestManager : public Susi::Autodiscovery::Manager {
	public:
		TestManager(std::string addr1,std::string addr2) : Susi::Autodiscovery::Manager{addr1,addr2} {}
	protected:
		virtual void onNewHost(std::string & addr) override {
			std::cout<<_ownAddr<<": "<<addr<<" appears online!"<<std::endl;
		}
	};

	TestManager manager1{"239.255.255.250:12345","Host1"};
	TestManager manager2{"239.255.255.250:12345","Host2"};
	TestManager manager3{"239.255.255.250:12345","Host3"};
	manager1.start();
	manager2.start();
	manager3.start();
	std::this_thread::sleep_for(std::chrono::seconds{2});
}