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

void Susi::Auth::EventInterface::init() {	
	Susi::Events::subscribe("auth::login", handleLogin);
	Susi::Events::subscribe("auth::logout", handleLogout);
	Susi::Events::subscribe("auth::isLoggedIn", handleIsLoggedIn);

	Susi::Events::subscribe("auth::getUsername", handleGetUsername);
}

void Susi::Auth::EventInterface::handleLogin(Susi::Events::EventPtr event) {
	try{
		std::string username = event->getPayload()["username"];
		std::string password  = event->getPayload()["password"];

		event->getPayload()["success"] = world.authController->login(event->getSessionID(), username, password);
	}catch(const std::exception & e){
		std::string msg = "Error in handleLogin(): ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}

void Susi::Auth::EventInterface::handleLogout(Susi::Events::EventPtr event) {
	try{
		world.authController->logout(event->getSessionID());		
	}catch(const std::exception & e){
		std::string msg = "Error in handleLogout(): ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}

void Susi::Auth::EventInterface::handleIsLoggedIn(Susi::Events::EventPtr event) {
	try{
		event->getPayload()["success"] = world.authController->isLoggedIn(event->getSessionID());
	}catch(const std::exception & e){
		std::string msg = "Error in handleIsLoggedIn(): ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}

void Susi::Auth::EventInterface::handleGetUsername(Susi::Events::EventPtr event) {
	try{
		event->getPayload()["username"] = world.authController->getUsername(event->getSessionID());
	}catch(const std::exception & e){
		std::string msg = "Error in handleIsLoggedIn(): ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}