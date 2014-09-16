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

#include "states/StateControllerComponent.h"

void Susi::States::StateControllerComponent::handleSave(Susi::Events::EventPtr event) {
	if(persistentChanged) {
		savePersistent();
		persistentChanged = false;
	}
}

void Susi::States::StateControllerComponent::handleSetState(Susi::Events::EventPtr event) {
	Susi::Logger::debug("got state event");
	try{
		std::string stateID  = event->payload["stateID"];
		Susi::Util::Any value = event->payload["value"];
		event->payload["success"] = setState(stateID, value);
	}catch(const std::exception & e){
		std::string msg = "Error in handleSetState(): ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}

void Susi::States::StateControllerComponent::handleGetState(Susi::Events::EventPtr event) {
	try{
		std::string stateID  = event->payload["stateID"];
		event->payload["value"] = getState(stateID);
	}catch(const std::exception & e){
		std::string msg = "Error in handleGetState(): ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}

void Susi::States::StateControllerComponent::handleSetPersistentState(Susi::Events::EventPtr event) {
	Susi::Logger::debug("got persitent state event");
	try{
		std::string stateID  = event->payload["stateID"];
		Susi::Util::Any value = event->payload["value"];
		event->payload["success"] = setPersistentState(stateID, value);
	}catch(const std::exception & e){
		std::string msg = "Error in handleSetPersistentState(): ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}

void Susi::States::StateControllerComponent::handleGetPersistentState(Susi::Events::EventPtr event) {
	try{
		std::string stateID  = event->payload["stateID"];
		event->payload["value"] = getPersistentState(stateID);
	}catch(const std::exception & e){
		std::string msg = "Error in handleGetPersistentState(): ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}