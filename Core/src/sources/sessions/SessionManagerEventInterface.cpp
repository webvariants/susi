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

#include "sessions/SessionManagerEventInterface.h"
#include "world/World.h"
using namespace Susi::Sessions;

void Susi::Sessions::initEventInterface() {
	Susi::Events::subscribe("session::setAttribute", handleSetAttribute);
	Susi::Events::subscribe("session::getAttribute", handleGetAttribute);
	Susi::Events::subscribe("session::pushAttribute", handlePushAttribute);
	Susi::Events::subscribe("session::removeAttribute", handleRemoveAttribute);
	Susi::Events::subscribe("session::update", handleUpdate);
	Susi::Events::subscribe("session::check", handleCheck);
}

void Susi::Sessions::handleGetAttribute(Susi::Events::EventPtr event) {
	try{
		std::string sessionID = event->payload["id"];
		std::string key = event->payload["key"];

		event->payload["value"] = world.sessionManager->getSessionAttribute(sessionID, key);		
	}catch(const std::exception & e){
		std::string msg = "Error in handleSessionGetAttribute(): ";
		msg += e.what();
		throw std::runtime_error(msg);		
	}
}
void Susi::Sessions::handleSetAttribute(Susi::Events::EventPtr event) {
	try{
		std::string sessionID = event->payload["id"];
		std::string key = event->payload["key"];
		Susi::Util::Any value = event->payload["value"];

		event->payload["success"] = world.sessionManager->setSessionAttribute(sessionID, key, value);
	}catch(const std::exception & e){
		std::string msg = "Error in handleSessionSetAttribute(): ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}
void Susi::Sessions::handlePushAttribute(Susi::Events::EventPtr event) {
	try{
		std::string sessionID = event->payload["id"];
		std::string key = event->payload["key"];
		Susi::Util::Any value = event->payload["value"];

		event->payload["success"] = world.sessionManager->pushSessionAttribute(sessionID, key, value);
	}catch(const std::exception & e){
		std::string msg = "Error in handleSessionPushAttribute(): ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}
void Susi::Sessions::handleRemoveAttribute(Susi::Events::EventPtr event) {
	try{
		std::string sessionID = event->payload["id"];
		std::string key = event->payload["key"];

		event->payload["success"] = world.sessionManager->removeSessionAttribute(sessionID, key);
	}catch(const std::exception & e){
		std::string msg = "Error in handleSessionRemoveAttribute(): ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}
void Susi::Sessions::handleUpdate(Susi::Events::EventPtr event) {
	try{
		std::string sessionID = event->payload["id"];
		std::chrono::seconds secs;

		int seconds = event->payload["seconds"];

		if(seconds > 0) {
			secs = std::chrono::seconds(seconds);
			world.sessionManager->updateSession(sessionID, std::chrono::duration_cast<std::chrono::milliseconds>(secs));
		}
		else {
			world.sessionManager->updateSession(sessionID);
		}

		event->payload["success"] = true;
	}catch(const std::exception & e){
		std::string msg = "Error in handleUpdateSession(): ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}
void Susi::Sessions::handleCheck(Susi::Events::EventPtr event) {
	try{
		std::string sessionID = event->payload["id"];
		event->payload["success"] = world.sessionManager->checkSession(sessionID);		
	}catch(const std::exception & e){
		std::string msg = "Error in handleCheckSession(): ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}
