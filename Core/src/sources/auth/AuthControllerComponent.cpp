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

#include "auth/AuthControllerComponent.h"

void Susi::Auth::ControllerComponent::handleLogin( Susi::Events::EventPtr event ) {
    try {
        std::string username = event->payload["username"];
        std::string password  = event->payload["password"];
        event->payload.reset();
        event->payload["success"] = login( event->getSessionID(), username, password );
    }
    catch( const std::exception & e ) {
        event->getPayload()["success"] = false;
        std::string msg = "Error in handleLogin(): ";
        msg += e.what();
        throw std::runtime_error( msg );
    }
}

void Susi::Auth::ControllerComponent::handleLogout( Susi::Events::EventPtr event ) {
    event->payload["success"] = logout( event->getSessionID() );
}

void Susi::Auth::ControllerComponent::handleIsLoggedIn( Susi::Events::EventPtr event ) {
    event->payload["success"] = isLoggedIn( event->getSessionID() );
}

void Susi::Auth::ControllerComponent::handleGetUsername( Susi::Events::EventPtr event ) {
    event->payload["username"] = getUsername( event->getSessionID() );
}

void Susi::Auth::ControllerComponent::handleAddUser( Susi::Events::EventPtr event ) {
    std::string username = event->payload["username"];
    std::string password  = event->payload["password"];
    long authlevel  = event->payload["authlevel"];
    event->payload.reset();
    event->payload["success"] = addUser( username,password,authlevel );
}

void Susi::Auth::ControllerComponent::handleDelUser( Susi::Events::EventPtr event ) {
    auto & payload = event->payload;
    event->payload["success"] = delUser( payload["username"] );
}

void Susi::Auth::ControllerComponent::handleUpdateUser( Susi::Events::EventPtr event ) {
    std::string username = event->payload["username"];
    std::string password  = event->payload["password"];
    long authlevel  = event->payload["authlevel"];
    event->payload.reset();
    event->payload["success"] = updateUser( username,password,authlevel );
}
