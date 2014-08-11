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

#include "enginestarter/EngineEventinterface.h"
 
void Susi::EngineStarter::EventInterface::init() {	
	Susi::Events::subscribe("enginestarter::start", handleStart);
	Susi::Events::subscribe("enginestarter::restart", handleRestart);
	Susi::Events::subscribe("enginestarter::stop", handleStop);

	Susi::Events::subscribe("global::start", handleStart);
	Susi::Events::subscribe("global::restart", handleRestart);
	Susi::Events::subscribe("global::stop", handleStop);
}

void Susi::EngineStarter::EventInterface::handleStart(Susi::Events::EventPtr event) {
	try{		
		world.engineStarter->execute();
	}catch(const std::exception & e){
		std::string msg = "Error in handleStart(): ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}

void Susi::EngineStarter::EventInterface::handleRestart(Susi::Events::EventPtr event) {
	try{		
		world.engineStarter->killall();
		world.engineStarter->execute();
	}catch(const std::exception & e){
		std::string msg = "Error in handleRestart(): ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}

void Susi::EngineStarter::EventInterface::handleStop(Susi::Events::EventPtr event) {
	try{		
		world.engineStarter->execute();
	}catch(const std::exception & e){
		std::string msg = "Error in handleStop(): ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}