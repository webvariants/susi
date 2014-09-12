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
#include <mutex>
#include <condition_variable>
#include "TinyJSEngine.h"

int main(int argc, char** argv){
	if(argc < 3){
		std::cout<<"usage : "<<argv[0]<<" <susi address> <source file>"<<std::endl;
		std::cout<<"sample: "<<argv[0]<<" [::1]:4000 sourcefile.js"<<std::endl;
		return 1;			
	}

	Susi::JS::Tiny::Engine engine{argv[1],argv[2]};

	bool stop = false;
	std::mutex mutex;
	std::condition_variable stop_cond;
	std::unique_lock<std::mutex> lock{mutex};
	stop_cond.wait(lock,[&stop](){return stop;});

}