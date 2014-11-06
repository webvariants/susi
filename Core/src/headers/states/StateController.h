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

#include "iocontroller/IOController.h"
#include "logger/easylogging++.h"
#include "util/Any.h"

namespace Susi {
    namespace States {
        class StateController {
        protected:
            std::map<std::string, Susi::Util::Any> volatileStates;
            std::map<std::string, Susi::Util::Any> persistentStates;

            std::mutex mutex;
            std::string fileLocation;
            bool persistentChanged = false;
        public:
            StateController( std::string file );
            bool setState( std::string stateID, Susi::Util::Any value );
            bool setPersistentState( std::string stateID, Susi::Util::Any value );
            Susi::Util::Any getState( std::string stateID );
            Susi::Util::Any getPersistentState( std::string stateID );
            bool removeState( std::string stateID );
            bool removePersistentState( std::string stateID );
            void savePersistent();
            bool loadPersistent();
            ~StateController();
        };
    }
}

#endif // __STATECONTROLLER__
