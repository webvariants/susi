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
	subscribe("session::setAttribute", handleSetAttribute);
	subscribe("session::getAttribute", handleGetAttribute);
	subscribe("session::pushAttribute", handlePushAttribute);
	subscribe("session::removeAttribute", handleRemoveAttribute);
	subscribe("session::update", handleUpdate);
	subscribe("session::check", handleCheck);
}

void Susi::Sessions::handleGetAttribute(Event & event) {
	try{
		std::string sessionID = event.payload["id"];
		std::string key = event.payload["key"];
		answerEvent(event, world.sessionManager->getSessionAttribute(sessionID, key));
	}catch(const std::exception & e){
		std::string msg = "Error in handleSessionGetAttribute(): ";
		msg += e.what();
		Susi::error(msg);
		answerEvent(event, false);
	}
}
void Susi::Sessions::handleSetAttribute(Event & event) {
	try{
		std::string sessionID = event.payload["id"];
		std::string key = event.payload["key"];
		Poco::Dynamic::Var value = event.payload["value"];
		answerEvent(event, world.sessionManager->setSessionAttribute(sessionID, key, value));
	}catch(const std::exception & e){
		std::string msg = "Error in handleSessionSetAttribute(): ";
		msg += e.what();
		Susi::error(msg);
		answerEvent(event, false);
	}
}
void Susi::Sessions::handlePushAttribute(Event & event) {
	try{
		std::string sessionID = event.payload["id"];
		std::string key = event.payload["key"];
		Poco::Dynamic::Var value = event.payload["value"];
		answerEvent(event, world.sessionManager->pushSessionAttribute(sessionID, key, value));
	}catch(const std::exception & e){
		std::string msg = "Error in handleSessionPushAttribute(): ";
		msg += e.what();
		Susi::error(msg);
		answerEvent(event, false);
	}
}
void Susi::Sessions::handleRemoveAttribute(Event & event) {
	try{
		std::string sessionID = event.payload["id"];
		std::string key = event.payload["key"];
		answerEvent(event, world.sessionManager->removeSessionAttribute(sessionID, key));
	}catch(const std::exception & e){
		std::string msg = "Error in handleSessionRemoveAttribute(): ";
		msg += e.what();
		Susi::error(msg);
		answerEvent(event, false);
	}
}
void Susi::Sessions::handleUpdate(Event & event) {
	try{
		std::string sessionID = event.payload["id"];
		std::chrono::seconds secs;
		if(event.payload["seconds"] > 0) {
			secs = std::chrono::seconds(event.payload["seconds"]);
			world.sessionManager->updateSession(sessionID, std::chrono::duration_cast<std::chrono::milliseconds>(secs));
		}
		else {
			world.sessionManager->updateSession(sessionID);
		}
		answerEvent(event, true);
	}catch(const std::exception & e){
		std::string msg = "Error in handleUpdateSession(): ";
		msg += e.what();
		Susi::error(msg);
		answerEvent(event, false);
	}
}
void Susi::Sessions::handleCheck(Event & event) {
	try{
		std::string sessionID = event.payload["id"];
		answerEvent(event, world.sessionManager->checkSession(sessionID));
	}catch(const std::exception & e){
		std::string msg = "Error in handleCheckSession(): ";
		msg += e.what();
		Susi::error(msg);
		answerEvent(event, false);
	}
}
