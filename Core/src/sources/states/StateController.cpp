
/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 *
 * @author: Christian Sonderfeld (christian.sonderfeld@webvariants.de), Thomas Krause (thomas.krause@webvariants.de)
 */

#include "susi/states/StateController.h"
using namespace Susi::States;

StateController::StateController( std::string file ) {
    // Todo: set fileLocation to a value
    this->fileLocation = file;
    try{
        loadPersistent();
    }catch(...){
        LOG(ERROR) << "can't load state file "+file;
    }
}

StateController::~StateController() {}

void StateController::savePersistent() {
    BSON::Value obj = persistentStates;
    Susi::IOController io{};
    io.writeFile( fileLocation,obj.toJSON() );
}

bool StateController::loadPersistent() {
    Susi::IOController io;
    std::string fileContent = io.readFile( fileLocation );
    LOG(INFO) << "read states from file...";
    persistentStates = BSON::Value::fromJSON( fileContent );
    return true;
}

bool StateController::setState( std::string stateID, BSON::Value value ) {

    std::lock_guard<std::mutex> lock( mutex );

    if( stateID.length() > 0 ) {
        volatileStates[stateID] = value;
        return true;
    }
    return false;
}

bool StateController::setPersistentState( std::string stateID, BSON::Value value ) {
    std::lock_guard<std::mutex> lock( mutex );

    if( stateID.length() > 0 ) {
        persistentStates[stateID] = value;
        persistentChanged = true;
        return true;
    }
    return false;
}

BSON::Value StateController::getState( std::string stateID ) {
    if( stateID.length() > 0 ) {
        return volatileStates[stateID];
    }
    return BSON::Value();
}

BSON::Value StateController::getPersistentState( std::string stateID ) {
    if( stateID.length() > 0 ) {
        return persistentStates[stateID];
    }
    return BSON::Value();
}

bool StateController::removeState( std::string stateID ) {
    std::lock_guard<std::mutex> lock( mutex );

    if( stateID.length() > 0 ) {
        return volatileStates.erase( stateID );
    }
    return false;
}
bool StateController::removePersistentState( std::string stateID ) {
    std::lock_guard<std::mutex> lock( mutex );
    if( stateID.length() > 0 ) {
        persistentChanged = persistentStates.erase( stateID ) == 1;
        return persistentChanged;
    }
    return false;
}
