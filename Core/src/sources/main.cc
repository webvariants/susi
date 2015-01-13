/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 * 
 * @author: Thomas Krause (thomas.krause@webvariants.de)
 */

#include <iostream>
#include <string>
#include <mutex>
#include <vector>
#include <condition_variable>
#include <csignal>
#include <iostream>
#include <zlib.h>

#include "config/Config.h"
#include "world/SusiServerComponentManager.h"

#include "logger/easylogging++.h"
INITIALIZE_EASYLOGGINGPP

std::condition_variable waitCond;

std::shared_ptr<Susi::System::SusiServerComponentManager> componentManager;

void waitForEver(){
	std::mutex mutex;
	std::unique_lock<std::mutex> lk(mutex);
	waitCond.wait(lk,[](){return false;});
}

void signalHandler (int signum) {
	std::cout << "Interrupt signal (" << signum << ") received.\n";
	componentManager->stopAll();
	exit(0);
}

int main(int argc, char** argv){
   	
	std::vector<std::string> argv_vec;

	for (int i=0; i<argc; i++) {
		argv_vec.push_back(argv[i]);
	}

	Susi::Config cfg{};
	cfg.parseCommandLine(argv_vec);

	try {
		std::string config_file = cfg.get("config");
		cfg.loadConfig(config_file);   		
   	} catch(const std::exception & e){
   		// default fallback
   		cfg.loadConfig("config.json");
   	}
   	
   	auto easylogFile = cfg.get("logger.easylogging++");
   	if(easylogFile.isString()){
   		el::Configurations conf(easylogFile);
   		el::Loggers::reconfigureAllLoggers(conf);
   	}

	componentManager.reset(new Susi::System::SusiServerComponentManager{cfg.getConfig()});
	componentManager->startAll();

	auto eventsystem = componentManager->getComponent<Susi::Events::IEventSystem>("eventsystem");
	auto event = eventsystem->createEvent("global::start");
	eventsystem->publish(std::move(event));

	// register signal SIGINT and signal handler  
    signal(SIGINT, signalHandler); 

	waitForEver();

	componentManager->stopAll();

	exit(0);
}