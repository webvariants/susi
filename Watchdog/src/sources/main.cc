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
#include <chrono>
#include <condition_variable>

#include <csignal>

#include "Watchdog.h"

std::condition_variable waitCond;

void waitForEver(){
	std::mutex mutex;
	std::unique_lock<std::mutex> lk(mutex);
	waitCond.wait(lk,[](){return false;});
}


void signalHandler (int signum) {
	std::cout << "Interrupt signal (" << signum << ") received.\n";
	exit(0);
}

int main(int argc, char** argv){

	// register signal SIGINT and signal handler  
    signal(SIGINT, signalHandler); 

	waitForEver();

	exit(0);
}