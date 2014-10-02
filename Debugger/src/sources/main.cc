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
#include <iostream>

#include "util/Any.h"

#include "CommandlineParser.h"

#include "DebuggerEngine.h"
#include "PrintController.h"

 using std::cout;

 void showHelp() {
	cout<<"Usage ..."<<std::endl;
	cout<<"susi-debugger <arguments for debugger> -- [Susi-Instance] \n\n";
	cout<<"susi-debugger arguments ...\n";
	cout<<"	-help            | -h \n";
	cout<<"	-verbose         | -v \n";
	cout<<"	-----------------------\n";
	cout<<"	-subscribe=topic | -s \n";	
	cout<<"	-publish=topic   | -p \n";	
	cout<<"	-payload=JSON    | -pa \n";	
	cout<<"\n\n";

	cout<<"subcribe example \n";
	cout<<" ./susi-debugger -s heartbeat::one \n";	

	cout<<"publish example \n";
	cout<<" ./susi-debugger -s test -p test -pa {\\\"foo\\\":\\\"bar\\\"} \n"<<std::endl;	
}

int main(int argc, char** argv) {

	Debugger::Parser parser;
	std::vector<std::string> debugger_args;
	std::vector<std::string> filtered_vec;

	std::condition_variable cond;
	std::mutex mutex;
	std::atomic<int> times(0);

	std::string susi_instance = "[::1]:4000"; // default


	// check help
	if(argc < 2) {		
		std::cout<<"use -h for help"<<std::endl;
		exit(0);
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
	
	parser.registerCommandLineOption("verbose", "verbose", "false");
	parser.registerCommandLineOption("help", "help", "false");
	parser.registerCommandLineOption("subscribe", "subscribe", "", "multi");
	parser.registerCommandLineOption("publish", "publish");
	parser.registerCommandLineOption("paypload", "paypload");

	// shortcuts
	parser.registerCommandLineOption("v", "verbose", "false");
	parser.registerCommandLineOption("h", "help", "false");
	parser.registerCommandLineOption("s", "subscribe", "", "multi");
	parser.registerCommandLineOption("p", "publish");
	parser.registerCommandLineOption("pa", "paypload");

	parser.parseCommandLine(debugger_args);

	if(parser.getValueByKey("help") != "false") {		
		showHelp();
		cout<<"	-----------------------\n";
	}

	if(parser.getValueByKey("verbose") != "false") {		
		parser.printParsedArgs();
		cout<<"	-----------------------\n";
	}

	// init Engine
	std::shared_ptr<Debugger::Engine> e{new Debugger::Engine{susi_instance}};

	std::string s_topic = parser.getValueByKey("subscribe");
	
	if(s_topic != "") {
		std::vector<std::string> elems;
		parser.split(s_topic, ' ', elems);

		for (size_t t=0; t < elems.size(); t++) {
    		std::cout << ' ' << elems[t] << std::endl;
			times.store(times.load() + 1);
			e->addController("printController", new Debugger::PrintController{elems[t], &times, &cond});
		}
		e->start();
	}

	std::string p_topic = parser.getValueByKey("publish");
	Susi::Util::Any payload{ Susi::Util::Any::fromJSONString(parser.getValueByKey("paypload"))};

	if(p_topic != "") {
		auto event = e->getApi()->createEvent(p_topic);
		 	 event->payload = payload;
		e->getApi()->publish(std::move(event));
	}
	
	std::cout<<"TIMES:"<<times.load()<<std::endl;
	
	std::unique_lock<std::mutex> lock{mutex};
	cond.wait(lock, [&times](){return times.load()==0;});
	
}
