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

#ifndef __AUTH_CONTROLLER__
#define __AUTH_CONTROLLER__

#include <soci.h>
#include <sqlite3/soci-sqlite3.h>
#include <iostream>
#include <map>
#include <tuple>
#include <memory>

#include <db/DBComponent.h>
#include <sessions/SessionManagerComponent.h>

#include "util/Any.h"
#include "util/sha3.h"

#include <algorithm>

namespace Susi {
    namespace Auth {
        class Controller {
        protected:
            std::string _dbIdentifier;
            std::shared_ptr<Susi::DB::DBComponent> _dbManager;
            std::shared_ptr<Susi::Sessions::SessionManagerComponent> _sessionManager;

            void setup();
            std::string generateSalt();

            bool checkForSqlSafety( std::string subject ) {
                auto pred = []( char c ) {
                    return !std::isalnum( c );
                };
                return std::find_if( subject.begin(),subject.end(),pred ) == subject.end();
            }

        public:
            Controller( std::shared_ptr<Susi::DB::DBComponent> dbManager,
                        std::shared_ptr<Susi::Sessions::SessionManagerComponent> sessionManager,
                        std::string db_identifier ) :
                    _dbIdentifier {db_identifier},
                    _dbManager {dbManager},
                    _sessionManager {sessionManager} {
                setup();
            }
            bool login( std::string sessionID, std::string username, std::string password ); // return true on success
            bool logout( std::string sessionID );
            bool isLoggedIn( std::string sessionID );

            bool addUser( std::string username, std::string password , char authlevel );
            bool updateUser( std::string username, std::string password , char authlevel );
            bool delUser( std::string username );

            std::string getUsername( std::string sessionID );
        };
    }
}

#endif // __AUTH_CONTROLLER__