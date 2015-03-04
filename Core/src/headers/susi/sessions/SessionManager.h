/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 *
 * @author: Tino Rusch (tino.rusch@webvariants.de), Thomas Krause (thomas.krause@webvariants.de)
 */

#ifndef __SESSIONMANAGER__
#define __SESSIONMANAGER__

#include <map>
#include <Poco/Timestamp.h>
#include <mutex>
#include <chrono>
#include <thread>
#include <iostream>

#include "susi/sessions/Session.h"
#include "susi/util/Any.h"
#include "susi/events/EventManager.h"

namespace Susi {
    namespace Sessions {
        class SessionManager {
        protected:
            std::map<std::string, Session> sessions;
            std::map<std::string, std::string> alias;
            std::mutex mutex;
            std::chrono::milliseconds stdLifetime;
        public:
            void resolveSessionID(std::string & sessionID){
                if(alias.count(sessionID)>0){
                    sessionID = alias[sessionID];
                }
            }
            bool checkSession( std::string id );
            int  checkSessions();
            void updateSession( std::string id );
            void updateSession( std::string id, std::chrono::milliseconds lifeTime );
            bool killSession( std::string id );
            bool setSessionAttribute( std::string sessionID, std::string key, Susi::Util::Any value );
            Susi::Util::Any getSessionAttribute( std::string sessionID, std::string key );
            void init( std::chrono::milliseconds stdSessionLifetime );
            bool pushSessionAttribute( std::string sessionID, std::string key, Susi::Util::Any value );
            bool removeSessionAttribute( std::string sessionID, std::string key );
            void addAlias(std::string alias, std::string sessionID);

            ~SessionManager();
        };
    }
}

#endif //__SESSIONMANAGER__
