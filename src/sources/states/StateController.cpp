/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 *
 * @author: Christian Sonderfeld (christian.sonderfeld@webvariants.de)
 */

#include "states/StateController.h"
using namespace Susi::States;

StateController::StateController(std::string file) {
	subId = Susi::subscribe("heartbeat::fiveMinute",[this](Susi::Event & event){
		if(this->persistentChanged) {
			this->savePersistent();
		}
	});
	// Todo: set fileLocation to a value
	this->fileLocation = file;
}

StateController::~StateController() {
	Susi::unsubscribe("heartbeat::fiveMinute",subId);
}

void StateController::savePersistent() {
	Poco::Dynamic::Struct<std::string> obj;
	for(auto & kv : persistentStates){
		obj[kv.first] = kv.second;
	}
	std::stringstream ss;
	Poco::JSON::Stringifier::stringify(obj,ss,0);
	std::string json = ss.str();
	Susi::IOController io;
	io.writeFile(fileLocation,json);
}

bool StateController::loadPersistent() {
	Susi::IOController io;
	std::string fileContent = io.readFile(fileLocation);
	Poco::JSON::Parser parser;
	Poco::Dynamic::Var data;
	try{
		data = parser.parse(fileContent);
	}catch(const std::exception & e){
		std::string msg = "cannot parse persistent state file: ";
		msg += e.what();
		Susi::error(msg);
		return false;
	}
	persistentStates = data.extract<std::map<std::string, Poco::Dynamic::Var>>();
	return true;
}

bool StateController::setState(std::string stateID, Poco::Dynamic::Var value){
	std::lock_guard<std::mutex> lock(mutex);
	if( stateID.length() > 0) {
		volatileStates[stateID] = value;
		return true;
	}
	return false;
}
bool StateController::setPersistentState(std::string stateID, Poco::Dynamic::Var value){
	std::lock_guard<std::mutex> lock(mutex);
	if( stateID.length() > 0) {
		persistentStates[stateID] = value;
		persistentChanged = true;
		return true;
	}
	return false;
}
Poco::Dynamic::Var StateController::getState(std::string stateID) {
	if(stateID.length() > 0) {
		return volatileStates.at(stateID);
	}
	return Poco::Dynamic::Var();
}
Poco::Dynamic::Var StateController::getPersistentState(std::string stateID) {
	if(stateID.length() > 0) {
		return persistentStates.at(stateID);
	}
	return Poco::Dynamic::Var();
}
bool StateController::removeState(std::string stateID){
	std::lock_guard<std::mutex> lock(mutex);
	if( stateID.length() > 0) {
		return volatileStates.erase(stateID);
	}
	return false;
}
bool StateController::removePersistentState(std::string stateID){
	std::lock_guard<std::mutex> lock(mutex);
	if( stateID.length() > 0) {
		persistentChanged = persistentStates.erase(stateID) == 1;
		return persistentChanged;
	}
	return false;
}
