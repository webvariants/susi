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

        try {
            std::string username = event->getPayload()["username"];
            std::string password  = event->getPayload()["password"];

            event->getPayload()["success"] = login( event->getSessionID(), username, password );
        }
        catch( const std::exception & e ) {
            event->getPayload()["success"] = false;
            throw e;
        }

    }
    catch( const std::exception & e ) {
        std::string msg = "Error in handleLogin(): ";
        msg += e.what();
        throw std::runtime_error( msg );
    }
}

void Susi::Auth::ControllerComponent::handleLogout( Susi::Events::EventPtr event ) {
    event->getPayload()["success"] = logout( event->getSessionID() );
}

void Susi::Auth::ControllerComponent::handleIsLoggedIn( Susi::Events::EventPtr event ) {
    event->getPayload()["success"] = isLoggedIn( event->getSessionID() );
}

void Susi::Auth::ControllerComponent::handleGetUsername( Susi::Events::EventPtr event ) {
    event->getPayload()["username"] = getUsername( event->getSessionID() );
}
