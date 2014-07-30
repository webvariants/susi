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

#include "auth/AuthEventInterface.h"
#include "world/World.h"

void Susi::Auth::EventInterface::init() {	
	subscribe("auth::login", handleLogin);
	subscribe("auth::logout", handleLogout);
	subscribe("auth::isLoggedIn", handleIsLoggedIn);

	subscribe("auth::getUsername", handleGetUsername);
}

void Susi::Auth::EventInterface::handleLogin(Event & event) {
	try{
		std::string username = event.payload["username"];
		std::string password  = event.payload["password"];
		answerEvent(event, world.authController->login(event.sessionID, username, password));
	}catch(const std::exception & e){
		std::string msg = "Error in handleLogin(): ";
		msg += e.what();
		Susi::error(msg);
		answerEvent(event, false);
	}
}

void Susi::Auth::EventInterface::handleLogout(Event & event) {
	try{
		world.authController->logout(event.sessionID);
		answerEvent(event, true);
	}catch(const std::exception & e){
		std::string msg = "Error in handleLogout(): ";
		msg += e.what();
		Susi::error(msg);
		answerEvent(event, false);
	}
}

void Susi::Auth::EventInterface::handleIsLoggedIn(Event & event) {
	try{
		answerEvent(event, world.authController->isLoggedIn(event.sessionID));
	}catch(const std::exception & e){
		std::string msg = "Error in handleIsLoggedIn(): ";
		msg += e.what();
		Susi::error(msg);
		answerEvent(event, false);
	}
}

void Susi::Auth::EventInterface::handleGetUsername(Event & event) {
	try{
		answerEvent(event, world.authController->getUsername(event.sessionID));
	}catch(const std::exception & e){
		std::string msg = "Error in handleIsLoggedIn(): ";
		msg += e.what();
		Susi::error(msg);
		answerEvent(event, false);
	}
}