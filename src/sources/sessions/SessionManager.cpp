/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 *
 * @authors: Tino Rusch (tino.rusch@webvariants.de), Christian Sonderfeld (christian.sondefeld@webvariants.de), Thomas Krause (thomas.krause@webvariants.de)
 */

#include "sessions/SessionManager.h"

using namespace Susi::Sessions;

bool SessionManager::init(std::chrono::milliseconds stdSessionLifetime, std::chrono::milliseconds checkInterval) {
	// Causes problem in tests, since we have a world object which does all the init stuff
	// Second init fails...
	if(initialized) {		
		return false;
	}

	initialized = true;
	if(stdLifetime.count() <= 0) {
		stdLifetime = std::chrono::milliseconds(1000);
	}
	this->stdLifetime = stdSessionLifetime;

	subId = Susi::Events::subscribe("heartbeat::one",[this](Susi::Events::EventPtr event){
		this->checkSessions();
	});	
	
	return true;
}


SessionManager::~SessionManager(){	
	Susi::Events::unsubscribe(subId);
}

int SessionManager::checkSessions(){
	std::lock_guard<std::mutex> lock(mutex);
	std::map<std::string, Session> newSessions;
	int deleted = 0;
	for(auto & kv : sessions){
		if(!kv.second.isDead()){
			newSessions[kv.first] = kv.second;
		}else{
			std::string topic = "session::die::";
			topic += kv.first;
			Susi::Event event(topic);
			Susi::publish(event);
			deleted++;
		}
	}
	sessions = newSessions;
	return deleted;
}

bool SessionManager::checkSession(std::string id){
	std::lock_guard<std::mutex> lock(mutex);

	try{
		return !sessions.at(id).isDead();
	}catch(const std::exception & e){
		return false;
	}
}

bool SessionManager::setSessionAttribute(std::string sessionID, std::string key, Susi::Util::Any value) {
	std::lock_guard<std::mutex> lock(mutex);
	if (sessionID.length() > 0)
	{
		if(sessions.count(sessionID) > 0) {
			this->sessions[sessionID].setAttribute(key, value);
		}
		else {
			Session session = Session(this->stdLifetime);
			session.setAttribute(key, value);
			sessions[sessionID] = session;
		}
		return true;
	}
	return false;
}

bool SessionManager::pushSessionAttribute(std::string sessionID, std::string key, Susi::Util::Any value) {
	std::lock_guard<std::mutex> lock(mutex);
	if (sessionID.length() > 0)
	{
		if(sessions.count(sessionID) > 0) {
			this->sessions[sessionID].pushAttribute(key, value);
		}
		else {
			Session session = Session(this->stdLifetime);
			session.setAttribute(key, value);
			sessions[sessionID] = session;
		}
		return true;
	}
	return false; 
}

bool SessionManager::removeSessionAttribute(std::string sessionID, std::string key) {
	std::lock_guard<std::mutex> lock(mutex);
	if (sessionID.length() > 0)
	{
		if(sessions.count(sessionID) > 0) {
			return this->sessions[sessionID].removeAttribute(key);
		}
	}
	return false;
}

Susi::Util::Any SessionManager::getSessionAttribute(std::string sessionID, std::string key) {
	std::lock_guard<std::mutex> lock(mutex);
	if (sessionID.length() > 0 && sessions.count(sessionID) > 0)
	{
		return sessions[sessionID].getAttribute(key);
	}
	return Susi::Util::Any();
}

void SessionManager::updateSession(std::string id){
	std::lock_guard<std::mutex> lock(mutex);
	if(sessions.count(id) > 0 && !sessions[id].isDead()) {
		sessions[id].addTime(stdLifetime);
	}
	else {
		sessions[id] = Session(this->stdLifetime);
	}
}

void SessionManager::updateSession(std::string id, std::chrono::milliseconds lifetime){
	std::lock_guard<std::mutex> lock(mutex);
	if(sessions.count(id) > 0) {
		sessions[id].addTime(lifetime);
	}
	else {
		sessions[id] = Session(lifetime);
	}
}

bool SessionManager::killSession(std::string id) {
	if(sessions.count(id) > 0) {
		return sessions[id].die();
	}
	return false;
}