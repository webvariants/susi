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

#include "helper.h"

#include "Poco/Process.h"

std::atomic<int> p_id (0);
std::atomic<int> ret_code (0);

std::atomic<bool> processStarted (false);
std::atomic<bool> processKillRequest (false);

std::thread t;


void signalHandler (int signum) {
	std::cout << "Interrupt signal (" << signum << ") received.\n";

	if(processStarted == true && p_id != 0) {
		processKillRequest = true;
		std::cout<<"processKillRequest"<<std::endl;
		// send signal to process
		Poco::Process::requestTermination(p_id);
	}
}


void startProcess(std::string program) {
	std::string cmd("bash");
	std::vector<std::string> args;
	args.push_back(program);
	
	Poco::ProcessHandle ph = Poco::Process::launch(cmd, args, 0, 0, 0);

	processStarted = true;

	p_id = ph.id();

	ret_code = ph.wait();	
}

int main(int argc, char** argv){

	signal(SIGINT, signalHandler);

	std::string cmd = "../test/test.sh";

	for(;;) {
		std::cout<<"start"<<std::endl;

		t = std::thread(startProcess, cmd);

		t.join();

		if(ret_code != 0) {
			break;
		}

		std::cout<<"thread finished"<<std::endl;
		if(processKillRequest == true) {
			std::cout<<"processKillRequest after"<<std::endl;
			exit(0);
		}
	}

	exit(0);
}