#include "util/ComponentTest.h"

class ApiServerComponentTest : public ComponentTest {
protected:
	std::shared_ptr<Susi::Api::ApiServerComponent> apiserver;
	bool called{false};
	std::mutex mutex;
	std::condition_variable cond;

	std::string sessionID{"123"};

	void waitForCondition(int timeout){
		std::unique_lock<std::mutex> lock{mutex};
		cond.wait_for(lock,std::chrono::milliseconds{timeout},[this](){return called;});
		EXPECT_EQ(true,called);
		called = false;
	}

	virtual void SetUp() override {
		componentManager->startComponent("apiserver");
		apiserver = componentManager->getComponent<Susi::Api::ApiServerComponent>("apiserver");
	}
	
	virtual void TearDown() override {
		componentManager->stopComponent("apiserver");
	}

	virtual void GoodCases() override {
		apiserver->registerSender(sessionID,[this](Susi::Util::Any & packet){
			Susi::Util::Any expect = Susi::Util::Any::Object{
				{"type","status"},
				{"error",false}
			};
			EXPECT_EQ(expect.toJSONString(),packet.toJSONString());
			called = true;
			cond.notify_one();
		});
		apiserver->onConnect(sessionID);

		Susi::Util::Any msg = Susi::Util::Any::Object{
			{"type","registerConsumer"},
			{"data",Susi::Util::Any::Object{
				{"topic","sample"}
			}}
		};
		apiserver->onMessage(sessionID,msg);
		waitForCondition(250);

		msg = Susi::Util::Any::Object{
			{"type","registerProcessor"},
			{"data",Susi::Util::Any::Object{
				{"topic","sample"}
			}}
		};
		apiserver->onMessage(sessionID,msg);
		waitForCondition(250);

		msg = Susi::Util::Any::Object{
			{"type","publish"},
			{"data",Susi::Util::Any::Object{
				{"topic","sample"}
			}}
		};

		Susi::Util::Any procEvent;

		apiserver->registerSender(sessionID,[this,&procEvent](Susi::Util::Any & packet){
			if(packet["type"]=="status"){
				
				Susi::Util::Any expect = Susi::Util::Any::Object{
					{"type","status"},
					{"error",false}
				};
				EXPECT_EQ(expect.toJSONString(),packet.toJSONString());
			}else if(packet["type"]=="ack"){
				EXPECT_EQ(Susi::Util::Any{"sample"},packet["data"]["topic"]);
				EXPECT_EQ(Susi::Util::Any{"123"},packet["data"]["sessionid"]);
			}else if(packet["type"]=="consumerEvent"){
				EXPECT_EQ(Susi::Util::Any{"sample"},packet["data"]["topic"]);
				EXPECT_EQ(Susi::Util::Any{"123"},packet["data"]["sessionid"]);
			}else if(packet["type"]=="processorEvent"){
				EXPECT_EQ(Susi::Util::Any{"sample"},packet["data"]["topic"]);
				EXPECT_EQ(Susi::Util::Any{"123"},packet["data"]["sessionid"]);
				procEvent = packet;
			}
			called = true;
			cond.notify_one();
		});
		apiserver->onMessage(sessionID,msg);
		waitForCondition(250);//status
		waitForCondition(250);//processorEvent
		procEvent["type"] = "ack";
		apiserver->onMessage(sessionID,procEvent);
		waitForCondition(250);//status
		waitForCondition(250);//consumerEvent
		waitForCondition(250);//ack

		msg = Susi::Util::Any::Object{
			{"type","unregisterConsumer"},
			{"data",Susi::Util::Any::Object{
				{"topic","sample"},
				{"authlevel",3}
			}}
		};
		apiserver->onMessage(sessionID,msg);
		waitForCondition(250);//status
		msg = Susi::Util::Any::Object{
			{"type","unregisterProcessor"},
			{"data",Susi::Util::Any::Object{
				{"topic","sample"},
				{"authlevel",3}
			}}
		};
		apiserver->onMessage(sessionID,msg);
		waitForCondition(250);//status

		apiserver->onClose(sessionID);
	}

	virtual void BadCases() override {
		auto statusSuccess = [this](Susi::Util::Any & packet){
			Susi::Util::Any expect = Susi::Util::Any::Object{
				{"type","status"},
				{"error",false}
			};
			EXPECT_EQ(expect.toJSONString(),packet.toJSONString());
			called = true;
			cond.notify_one();
		};
		auto statusError = [this](Susi::Util::Any & packet){
			EXPECT_EQ(Susi::Util::Any{"status"},packet["type"]);
			EXPECT_EQ(Susi::Util::Any{true},packet["error"]);
			called = true;
			cond.notify_one();
		};

		apiserver->registerSender(sessionID,statusSuccess);
		apiserver->onConnect(sessionID);
		Susi::Util::Any msg = Susi::Util::Any::Object{
			{"type","registerConsumer"},
			{"data",Susi::Util::Any::Object{
				{"topic","sample"}
			}}
		};
		apiserver->onMessage(sessionID,msg);
		waitForCondition(250);
		apiserver->registerSender(sessionID,statusError);
		apiserver->onMessage(sessionID,msg);
		waitForCondition(250);


		apiserver->registerSender(sessionID,statusSuccess);
		apiserver->onConnect(sessionID);
		msg = Susi::Util::Any::Object{
			{"type","registerProcessor"},
			{"data",Susi::Util::Any::Object{
				{"topic","sample"}
			}}
		};
		apiserver->onMessage(sessionID,msg);
		waitForCondition(250);
		apiserver->registerSender(sessionID,statusError);
		apiserver->onMessage(sessionID,msg);
		waitForCondition(250);

		msg = Susi::Util::Any::Object{
			{"type","wrong"},
			{"data","sample"}
		};
		apiserver->onMessage(sessionID,msg);
		waitForCondition(250);

		msg = Susi::Util::Any::Object{
			{"type",1},
			{"data","sample"}
		};
		apiserver->onMessage(sessionID,msg);
		waitForCondition(250);

		msg = Susi::Util::Any::Object{};
		apiserver->onMessage(sessionID,msg);
		waitForCondition(250);

		msg = Susi::Util::Any::Object{
			{"type","registerConsumer"},
			{"data",false}
		};
		apiserver->onMessage(sessionID,msg);
		waitForCondition(250);

		msg = Susi::Util::Any::Object{
			{"type","registerProcessor"},
			{"data",false}
		};
		apiserver->onMessage(sessionID,msg);
		waitForCondition(250);

		msg = Susi::Util::Any::Object{
			{"type","publish"},
			{"data",false}
		};
		apiserver->onMessage(sessionID,msg);
		waitForCondition(250);

		msg = Susi::Util::Any::Object{
			{"type","ack"},
			{"data",false}
		};
		apiserver->onMessage(sessionID,msg);
		waitForCondition(250);

		msg = Susi::Util::Any::Object{
			{"type","unregisterConsumer"},
			{"data","foo"}
		};
		apiserver->onMessage(sessionID,msg);
		waitForCondition(250);//status
		msg = Susi::Util::Any::Object{
			{"type","unregisterProcessor"},
			{"data","foo"}
		};
		apiserver->onMessage(sessionID,msg);
		waitForCondition(250);//status
	}

	virtual void EdgeCases() override {

	}

};

TEST_F(ApiServerComponentTest,GoodCases){
	GoodCases();
}

TEST_F(ApiServerComponentTest,BadCases){
	BadCases();
}

TEST_F(ApiServerComponentTest,EdgeCases){
	EdgeCases();
}
