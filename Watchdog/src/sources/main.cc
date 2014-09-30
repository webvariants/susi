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

#include <atomic>
#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <csignal>

#include "config.h"

#include "Poco/Process.h"
#include "Poco/Foundation.h"

std::atomic<int> p_id (0);
std::atomic<int> ret_code (0);

std::atomic<bool> processStarted (false);
std::atomic<bool> processKillRequest (false);

std::thread t;
std::string prog = "";
Config config;


void signalHandler (int signum) {
	std::cout << "Interrupt signal (" << signum << ") received.\n";

	if(processStarted == true && p_id != 0) {
		processKillRequest = true;		
		// send signal to process
		if(config.kill_friendly) {
			Poco::Process::requestTermination(p_id);
		} else {
			Poco::Process::kill(p_id);
		}
	}
}

void init()  {
	processStarted = false;
	processKillRequest = false;
	p_id = 0;
	ret_code = 0;
}


void startProcess(std::string program, std::vector<std::string> args) {	
	Poco::ProcessHandle ph = Poco::Process::launch(program, args, 0, 0, 0);
	processStarted = true;
	p_id = ph.id();
	ret_code = ph.wait();	
}

int main(int argc, char** argv){
	
	// register signal handler
	signal(SIGINT,  signalHandler); // Strg-C
	signal(SIGUSR1, signalHandler); // 10
	signal(SIGUSR2, signalHandler); // 12

	std::vector<std::string> watchdog_args;
	std::vector<std::string> filtered_vec;
	std::vector<std::string> programm_args;

	// check help
	if(argc < 2) {
		std::cout<<"Usage ..."<<std::endl;
		std::cout<<"watchdog <arguments for watchdog> -- [PATH TO EXECUTABLE] <arguments for executable> \n"<<std::endl;
		std::cout<<"watchdog arguments ...>"<<std::endl;
		std::cout<<"	-wd_f=[true,false]		kill process friendly with signal or hard kill>"<<std::endl;
		std::cout<<"	-wd_r=[times]		    restart process n times after finish, -1 means infined restarts>"<<std::endl;
		std::cout<<"\n\n>"<<std::endl;
		std::cout<<"Example for susi ...\n"<<std::endl;
		std::cout<<"./watchdog  ../../Core/build/susi --wd_f=true -config=\"../../Core/config.json\""<<std::endl;
		std::cout<<"Example for test.sh ...\n"<<std::endl;
		std::cout<<"./watchdog  ../test/test.sh -wd_r=2"<<std::endl;
		exit(0);
	}


	// get arguments
	bool delimeter_found = false; // '--'

	for (int i=1; i<argc; i++) {
		std::string option = argv[i];

		if(option.length() == 2 && option[0] == '-' && option[1] == '-') {
			delimeter_found = true;
			i++;

			if(i < argc) { prog = argv[i]; i++;	}
		}

		if(i < argc) {
			if(delimeter_found) { programm_args.push_back(argv[i]); }
			else{ watchdog_args.push_back(argv[i]);	}
		}
		
	}	

	// check programm
	if(!config.getExecutable(prog)) {
		std::cout<<"Process not found or isn't executable"<<std::endl;
		exit(0);
	}	

	// check watchdog arguments
	filtered_vec = config.parseCommandLine(watchdog_args);

	if(filtered_vec.size() > 0)	 {
		std::cout<<"Some Watchdog commands could not be processed!"<<std::endl;
		config.printArgs(filtered_vec);
		exit(0);
	}

	std::cout<<"Watchdog .."<<std::endl;
	std::cout<<"        PROGRAM: "<<prog<<std::endl;
	std::cout<<"  KILL_FRIENDLY: "<<((config.kill_friendly == true) ? "true" : "false")<<std::endl;
	std::cout<<"  RESTART_TRIES: "<<((config.restart_trys == -1) ? "infinite" : std::to_string(config.restart_trys))<<std::endl;

	for(;;) {
		init();

		std::cout<<"Start Process: "<<prog<<std::endl;

		t = std::thread(startProcess, prog, filtered_vec);
		t.join();

		if(ret_code != 0) {
			break;
		}

		std::cout<<"Thread finished with:"<<ret_code<<std::endl;
		if(processKillRequest == true) {
			std::cout<<"processKillRequest after"<<std::endl;
			exit(0);
		}

		if(config.restart_trys > 0 && config.restart_trys != -1) {
			config.restart_trys--;
		} else {
			if(config.restart_trys == 0) {
				std::cout<<"Watchdog finished."<<std::endl;
				exit(0);
			}
		}
	}		
	

	exit(0);
}