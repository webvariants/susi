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

#include "syscall/SysCallEventInterface.h"
#include "world/World.h"

void Susi::Syscall::EventInterface::init() {	
	Susi::Events::subscribe("syscall::startProcess", handleStartProcess);
}

void Susi::Syscall::EventInterface::handleStartProcess(Susi::Events::EventPtr event) {
	
	try{		
		std::string process_type = event->payload["process_type"];
		std::map<std::string, std::string> argsReplace = event->payload["argsReplace"];

		event->payload["success"] = world.syscallController->startProcess(process_type, argsReplace);				
	}catch(const std::exception & e){
		std::string msg = "Error in handleStartProcess(): ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}