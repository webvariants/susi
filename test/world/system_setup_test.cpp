#include "gtest/gtest.h"
#include "world/system_setup.h"

#include "events/global.h"

TEST(SystemSetup,Basic){
	Susi::Util::Any::Object config{
		{"eventmanager",Susi::Util::Any::Object{
			{"workers",4},
			{"queuelen",32}
		}},
		{"heartbeat",Susi::Util::Any::Object{}},
	};
	auto componentManager = Susi::System::createSusiComponentManager(config);
	componentManager->startAll();
	std::this_thread::sleep_for(std::chrono::milliseconds{100});
}