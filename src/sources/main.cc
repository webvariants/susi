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

#include "config/Config.h"
#include "world/ComponentManager.h"
#include "world/system_setup.h"

std::condition_variable waitCond;

std::shared_ptr<Susi::System::ComponentManager> componentManager;

void waitForEver()
{
	std::mutex mutex;
	std::unique_lock<std::mutex> lk(mutex);
	waitCond.wait(lk,[](){return false;});
}


void signalHandler( int signum )
{
    std::cout << "Interrupt signal (" << signum << ") received.\n";
   // cleanup and close up stuff here  
   // terminate program  
   componentManager->stopAll();
   exit(0);  

}


int main(int argc, char** argv){


	std::vector<std::string> argv_vec;

	for (int i=0; i<argc; i++) {
		argv_vec[i] = argv[i];
	}



	Susi::Config cfg{"config.json"};
	cfg.parseCommandLine(argv_vec);

	componentManager = Susi::System::createSusiComponentManager(cfg.getConfig());
	//componentManager = std::make_shared<Susi::System::ComponentManager>(cfg.getConfig());
	componentManager->startAll();

	// register signal SIGINT and signal handler  
    signal(SIGINT, signalHandler); 

	waitForEver();

	componentManager->stopAll();

	exit(0);
}