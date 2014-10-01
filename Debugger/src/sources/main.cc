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

#include "CommandlineParser.h"

#include "DebuggerEngine.h"
#include "PrintController.h"

 using std::cout;

 void showHelp() {
	cout<<"Usage ..."<<std::endl;
	cout<<"susi-debugger <arguments for debugger> -- [Susi-Instance] \n\n";
	cout<<"susi-debugger arguments ...\n";
	cout<<"	-subscribe=topic	\n";	
	cout<<"\n\n";	
}

int main(int argc, char** argv) {

	Debugger::Parser parser;
	std::vector<std::string> debugger_args;
	std::vector<std::string> filtered_vec;

	std::string susi_instance = "[::1]:4000"; // default
	int times = 0; // wait for n times events

	// check help
	if(argc < 2) {		
		std::cout<<"use -h for help"<<std::endl;
		exit(0);
	}

	if(argc == 2) {
		std::string o = argv[1];
		if(o == "-h" || o == "-help" || o == "--h" || o == "--help") {
			showHelp();
			exit(0);
		}
	}

	// get arguments
	bool delimeter_found = false; // '--'

	for (int i=1; i<argc; i++) {
		std::string option = argv[i];

		if(option == "--") {
			delimeter_found = true;
			i++;

			if(i < argc) { susi_instance = argv[i]; i++; }
		}

		if(i < argc) {
			if(delimeter_found) { 
				std::cout<<"Unused argument:"<<argv[i]<<std::endl;
			}
			else{ debugger_args.push_back(argv[i]);	}
		}
		
	}


	parser.registerCommandLineOption("times", "nz", "0");
	parser.registerCommandLineOption("subscribe", "sub");
	parser.registerCommandLineOption("publish", "pub");

	// shortcuts
	parser.registerCommandLineOption("t", "nz", "0");
	parser.registerCommandLineOption("s", "sub");
	parser.registerCommandLineOption("p", "pub");

	parser.parseCommandLine(debugger_args);

	parser.printParsedArgs();


	// init Engine
	std::shared_ptr<Debugger::Engine> e{new Debugger::Engine{susi_instance}};

	std::string s_topic = parser.getValueByKey("sub");
	times = std::stoi(parser.getValueByKey("nz"));
	
	if(s_topic != "") {
		e->addController("printController", new Debugger::PrintController{s_topic, times});
		e->start();
	}

	std::string p_topic = parser.getValueByKey("pub");

	if(p_topic != "") {
		e->getApi()->publish(std::move(e->getApi()->createEvent(p_topic)));
	}
	

	std::condition_variable cond;
	std::mutex mutex;
	std::unique_lock<std::mutex> lock{mutex};
	cond.wait(lock, [](){return false;});
}
