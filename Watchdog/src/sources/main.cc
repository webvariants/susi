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
#include "IOController.h"

#include "Poco/Process.h"
#include "Poco/Foundation.h"

std::atomic<int> p_id (0);
std::atomic<int> ret_code (0);

std::atomic<bool> processStarted (false);
std::atomic<bool> processKillRequest (false);

std::thread t;

IOController io;
std::string prog;


void signalHandler (int signum) {
	std::cout << "Interrupt signal (" << signum << ") received.\n";

	if(processStarted == true && p_id != 0) {
		processKillRequest = true;
		std::cout<<"processKillRequest"<<std::endl;
		// send signal to process
		Poco::Process::requestTermination(p_id);
	}
}


void startProcess(std::string program, std::vector<std::string> args) {	
	Poco::ProcessHandle ph = Poco::Process::launch(program, args, 0, 0, 0);
	processStarted = true;
	p_id = ph.id();
	ret_code = ph.wait();	
}

int main(int argc, char** argv){

	signal(SIGINT, signalHandler);

	std::vector<std::string> argv_vec;

	if(argc < 1) {
		std::cout<<"Help ..."<<std::endl;
	} else {

		prog = argv[1]; 

		if(io.getExecutable(prog)) {		

			// get argument 
			for (int i=2; i<argc; i++)	argv_vec.push_back(argv[i]);				

			for(;;) {
				std::cout<<"start"<<std::endl;

				t = std::thread(startProcess, prog, argv_vec);

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


		} else {
			std::cout<<"Prog not found or isn't executable"<<std::endl;
			exit(0);
		}

	}

	exit(0);
}