#include "util/ComponentTest.h"
#include <iostream>

class SessionManagerComponentTest : public ComponentTest {
protected:
	std::string session{"supertestsession"};
	virtual void SetUp() override {
		componentManager->startComponent("sessionmanager");
	}

	virtual void TearDown() override {
		componentManager->stopComponent("sessionmanager");
	}

	virtual void GoodCases() override {
		// get the session started
		try {
			Susi::Events::EventPtr evt = std::move(createEvent("session::update"));
			Susi::Util::Any payload{};
			payload["id"] = session;
			evt->setPayload(payload);
			auto result = publish_sync(std::move(evt));
			EXPECT_TRUE(result->getPayload()["success"]);
		} catch(std::exception& e) {
			std::cout<<"Error In SessionManagerComponentTest: "<<std::endl;
			std::cout<<e.what()<<std::endl;
		}
	}

	virtual void BadCases() override {

	}

	virtual void EdgeCases() override {

	}

};

TEST_F(SessionManagerComponentTest,GoodCases){
	GoodCases();
}

TEST_F(SessionManagerComponentTest,BadCases){
	BadCases();
}

TEST_F(SessionManagerComponentTest,EdgeCases){
	EdgeCases();
}
