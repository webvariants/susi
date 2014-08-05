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

#include "states/StateEventInterface.h"
 
void Susi::States::EventInterface::init() {	
	Susi::Events::subscribe("state::setState", handleSetState);
	Susi::Events::subscribe("state::getState", handleGetState);
	Susi::Events::subscribe("state::setPersistentState", handleSetPersistentState);
	Susi::Events::subscribe("state::getPersistentState", handleGetPersistentState);
}

void Susi::States::EventInterface::handleSetState(Susi::Events::EventPtr event) {
	try{
		std::string stateID  = event->payload["stateID"];
		Susi::Util::Any value = event->payload["value"];
		event->payload["success"] = world.stateController->setState(stateID, value);
	}catch(const std::exception & e){
		std::string msg = "Error in handleSetState(): ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}

void Susi::States::EventInterface::handleGetState(Susi::Events::EventPtr event) {
	try{
		std::string stateID  = event->payload["stateID"];		
		event->payload["value"] = world.stateController->getState(stateID);
	}catch(const std::exception & e){
		std::string msg = "Error in handleGetState(): ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}

void Susi::States::EventInterface::handleSetPersistentState(Susi::Events::EventPtr event) {
	try{
		std::string stateID  = event->payload["stateID"];
		Susi::Util::Any value = event->payload["value"];
		event->payload["success"] = world.stateController->setPersistentState(stateID, value);
	}catch(const std::exception & e){
		std::string msg = "Error in handleSetPersistentState(): ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}

void Susi::States::EventInterface::handleGetPersistentState(Susi::Events::EventPtr event) {
	try{
		std::string stateID  = event->payload["stateID"];		
		event->payload["value"] = world.stateController->getPersistentState(stateID);
	}catch(const std::exception & e){
		std::string msg = "Error in handleGetPersistentState(): ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}