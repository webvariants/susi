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

#include "susi/auth/AuthControllerComponent.h"

void Susi::Auth::ControllerComponent::handleLogin( Susi::Events::EventPtr event ) {
    try {
        std::string id = event->getSessionID();
        sessionManager->resolveSessionID(id);
        std::string username = event->payload["username"];
        std::string password  = event->payload["password"];
        event->payload.reset();
        event->payload["success"] = login( id, username, password );
    }
    catch( const std::exception & e ) {
        event->getPayload()["success"] = false;
        std::string msg = "Error in handleLogin(): ";
        msg += e.what();
        throw std::runtime_error( msg );
    }
}

void Susi::Auth::ControllerComponent::handleLogout( Susi::Events::EventPtr event ) {
    std::string id = event->getSessionID();
    sessionManager->resolveSessionID(id);
    event->payload["success"] = logout( id );
}

void Susi::Auth::ControllerComponent::handleIsLoggedIn( Susi::Events::EventPtr event ) {
    std::string id = event->getSessionID();
    sessionManager->resolveSessionID(id);
    auto username = sessionManager->getSessionAttribute(id,"User");
    if(username.isObject() && username["username"].isString()){
        event->payload["username"] = username["username"];
    }
    event->payload["success"] = isLoggedIn( id );
}

void Susi::Auth::ControllerComponent::handleGetUsername( Susi::Events::EventPtr event ) {
    std::string id = event->getSessionID();
    sessionManager->resolveSessionID(id);
    event->payload["username"] = getUsername( id );
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


void Susi::Auth::ControllerComponent::handleUpdateUsername( Susi::Events::EventPtr event ){
    std::string name = event->payload["username"];
    std::string newName = event->payload["newname"];
    event->payload.reset();
    event->payload["success"] = updateUsername(name,newName);
}
void Susi::Auth::ControllerComponent::handleUpdateAuthlevel( Susi::Events::EventPtr event ){
    std::string name = event->payload["username"];
    int authlevel = event->payload["authlevel"];
    event->payload.reset();
    event->payload["success"] = updateAuthlevel(name,(char)authlevel);
}
void Susi::Auth::ControllerComponent::handleUpdatePassword( Susi::Events::EventPtr event ){
    std::string name = event->payload["username"];
    std::string password = event->payload["password"];
    event->payload.reset();
    event->payload["success"] = updatePassword(name,password);
}

