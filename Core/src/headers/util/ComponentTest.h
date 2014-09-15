/*
  * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
  *
  * This file is released under the terms of the MIT license. You can find the
  * complete text in the attached LICENSE file or online at:
  *
  * http://www.opensource.org/licenses/mit-license.php
  * 
  * @author: Tino Rusch (tino.rusch@webvariants.de)
  */
 
#ifndef __COMPONENTTEST__
#define __COMPONENTTEST__
 
#include "gtest/gtest.h"
#include "world/system_setup.h"
#include "config/Config.h"
#include "iocontroller/IOController.h"
#include "logger/Logger.h"
#include "events/EventManagerComponent.h"

class ComponentTest : public ::testing::Test {
public:
	ComponentTest(){
		Susi::Logger::setLevel(0);
		Susi::IOController io;
		io.writeFile("/tmp/config.json",configString);
		Susi::Config cfg{"/tmp/config.json"};
		io.deletePath("/tmp/config.json");
		componentManager = Susi::System::createSusiComponentManager(cfg.getConfig());
		eventManager = componentManager->getComponent<Susi::Events::ManagerComponent>("eventsystem");
	}
protected:
	std::shared_ptr<Susi::System::ComponentManager> componentManager;
	std::shared_ptr<Susi::Events::ManagerComponent> eventManager;

	virtual void GoodCases() = 0;
	virtual void BadCases() = 0;
	virtual void EdgeCases() = 0;


	std::string configString ="{"
								"\"eventsystem\" : {"
									"\"threads\": 4,"
									"\"queuelen\": 32"
								"},"
								"\"heartbeat\": {"
									"\"fix\":\"me\""
								"},"
								"\"dbmanager\": ["
									"{"
									"\"identifier\" : \"auth\","
									"\"type\": \"sqlite3\","
									"\"uri\" : \"./auth.sqlite3\""
									"}"
								"],"
								"\"authcontroller\": {"
									"\"db\": \"auth\""
								"},"
								"\"tcpapiserver\": {"
									"\"address\": \"[::1]:4000\","
									"\"threads\": 4,"
									"\"backlog\": 32"
								"},"
								"\"enginestarter\": {"
									"\"path\": \"./engines\""
								"},"
								"\"iocontroller\": {"
									"\"base\": \".\""
								"},"
								"\"sessionmanager\": {"
									"\"lifetime\": 600000"
								"},"
								"\"statecontroller\": {"
									"\"file\": \"./states.json\""
								"},"
								"\"syscallcontroller\": {"
									"\"threads\": 4,"
									"\"queuelen\": 16,"
									"\"commands\": {"
										"\"ECHO\" : \"echo $arg\","
										"\"PWD\"  : \"pwd\""
									"}"
								"},"
								"\"httpserver\": {"
									"\"address\": \"[::1]:8080\","
									"\"assets\": \"./assets\""
								"}"
							"}";
	long subscribe(std::string topic, Susi::Events::Processor processor){
		long id = eventManager->subscribe(topic,std::move(processor));
		return id;
	}
	long subscribe(Susi::Events::Predicate pred, Susi::Events::Processor processor){
		long id = eventManager->subscribe(pred,std::move(processor));
		return id;
	}
	long subscribe(std::string topic, Susi::Events::Consumer consumer){
		long id = eventManager->subscribe(topic,std::move(consumer));
		return id;
	}
	long subscribe(Susi::Events::Predicate pred, Susi::Events::Consumer consumer){
		long id = eventManager->subscribe(pred,std::move(consumer));
		return id;
	}
	bool unsubscribe(long id){
		return eventManager->unsubscribe(id);
	}
	Susi::Events::EventPtr createEvent(std::string topic){
		return eventManager->createEvent(topic);
	}

	Susi::Events::SharedEventPtr waitForConsumerEvent(std::string topic,std::chrono::milliseconds timeout){
		callbackCalled = false;
		sharedEvent.reset();
		Susi::Events::Consumer consumer = [this](Susi::Events::SharedEventPtr event){
			onConsumerEvent(event);
		};
		long id = subscribe(topic,consumer);
		{
			std::unique_lock<std::mutex> lock{mutex};
			cv.wait_for(lock,timeout,[this](){return callbackCalled;});
		}
		unsubscribe(id);
		return sharedEvent;
	}


	Susi::Events::EventPtr waitForProcessorEvent(std::string topic,std::chrono::milliseconds timeout){
		callbackCalled = false;
		event.reset();
		Susi::Events::Processor processor = [this](Susi::Events::EventPtr event){
			onProcessorEvent(std::move(event));
		};
		long id = subscribe(topic,processor);
		{
			std::unique_lock<std::mutex> lock{mutex};
			cv.wait_for(lock,timeout,[this](){return callbackCalled;});
		}
		unsubscribe(id);
		return std::move(event);
	}

private:
	Susi::Events::EventPtr event;
	Susi::Events::SharedEventPtr sharedEvent;
	std::condition_variable cv;
	std::mutex mutex;
	bool callbackCalled;

	void onConsumerEvent(Susi::Events::SharedEventPtr evt){
		sharedEvent = evt;
		callbackCalled = true;
		cv.notify_one();
	}
	void onProcessorEvent(Susi::Events::EventPtr evt){
		event = std::move(evt);
		callbackCalled = true;
		cv.notify_one();
	}


};


 
#endif // __COMPONENTTEST__
  