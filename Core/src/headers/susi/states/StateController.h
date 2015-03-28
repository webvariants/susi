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

#ifndef __STATECONTROLLER__
#define __STATECONTROLLER__

#include <map>
#include <mutex>
#include <sstream>

#include "susi/iocontroller/IOController.h"
#include "susi/logger/easylogging++.h"
#include "bson/Value.h"

namespace Susi {
    namespace States {
        class StateController {
        protected:
            std::map<std::string, BSON::Value> volatileStates;
            std::map<std::string, BSON::Value> persistentStates;

            std::mutex mutex;
            std::string fileLocation;
            bool persistentChanged = false;
        public:
            StateController( std::string file );
            bool setState( std::string stateID, BSON::Value value );
            bool setPersistentState( std::string stateID, BSON::Value value );
            BSON::Value getState( std::string stateID );
            BSON::Value getPersistentState( std::string stateID );
            bool removeState( std::string stateID );
            bool removePersistentState( std::string stateID );
            void savePersistent();
            bool loadPersistent();
            ~StateController();
        };
    }
}

#endif // __STATECONTROLLER__
