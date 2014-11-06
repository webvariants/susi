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

void Susi::States::StateControllerComponent::handleAutoSave( Susi::Events::EventPtr event ) {
    if( persistentChanged ) {
        handleSave( std::move( event ) );
    }
}

void Susi::States::StateControllerComponent::handleSave( Susi::Events::EventPtr event ) {
    if( persistentChanged ) {
        try {
            savePersistent();
            persistentChanged = false;
            event->payload["success"] = true;
        }
        catch( const std::exception & e ) {
            event->payload["success"] = false;
            std::string msg = "Error in handleSave(): ";
            msg += e.what();
            throw std::runtime_error( msg );
        }
    }
    else {
        event->payload["success"] = false;
    }
}

void Susi::States::StateControllerComponent::handleSetState( Susi::Events::EventPtr event ) {
    LOG(DEBUG) <<  "got state event" ;
    try {
        std::string stateID  = event->payload["stateID"];
        Susi::Util::Any value = event->payload["value"];

        if( value.isNull() ) {
            event->payload["success"] = false;
        }
        else {
            event->payload["success"] = setState( stateID, value );
        }

    }
    catch( const std::exception & e ) {
        event->payload["success"] = false;
        std::string msg = "Error in handleSetState(): ";
        msg += e.what();
        throw std::runtime_error( msg );
    }
}

void Susi::States::StateControllerComponent::handleGetState( Susi::Events::EventPtr event ) {
    try {
        std::string stateID  = event->payload["stateID"];
        Susi::Util::Any value = getState( stateID );
        event->payload["value"] = value;
        if( value.isNull() ) {
            event->payload["success"] = false;
        }
        else {
            event->payload["success"] = true;
        }
    }
    catch( const std::exception & e ) {
        event->payload["success"] = false;
        std::string msg = "Error in handleGetState(): ";
        msg += e.what();
        throw std::runtime_error( msg );
    }
}

void Susi::States::StateControllerComponent::handleSetPersistentState( Susi::Events::EventPtr event ) {
    LOG(DEBUG) <<  "got persitent state event" ;
    try {
        std::string stateID  = event->payload["stateID"];
        Susi::Util::Any value = event->payload["value"];

        if( value.isNull() ) {
            event->payload["success"] = false;
        }
        else {
            event->payload["success"] = setPersistentState( stateID, value );
        }
    }
    catch( const std::exception & e ) {
        event->payload["success"] = false;
        std::string msg = "Error in handleSetPersistentState(): ";
        msg += e.what();
        throw std::runtime_error( msg );
    }
}

void Susi::States::StateControllerComponent::handleGetPersistentState( Susi::Events::EventPtr event ) {
    try {
        std::string stateID  = event->payload["stateID"];
        Susi::Util::Any value = getPersistentState( stateID );
        event->payload["value"] = value;
        if( value.isNull() ) {
            event->payload["success"] = false;
        }
        else {
            event->payload["success"] = true;
        }
    }
    catch( const std::exception & e ) {
        event->payload["success"] = false;
        std::string msg = "Error in handleGetPersistentState(): ";
        msg += e.what();
        throw std::runtime_error( msg );
    }
}