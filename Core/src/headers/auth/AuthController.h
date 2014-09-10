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

#include <db/Database.h>

#include "util/Any.h"

namespace Susi {
	namespace Auth {
		class Controller{
				std::string db_identifier;
			public:
				Controller(std::string db_identifier);				

				bool login(std::string sessionID, std::string username, std::string password); // return true on success
				void logout(std::string sessionID);
				bool isLoggedIn(std::string sessionID); 
				
				std::string getUsername(std::string sessionID);
		};
	}
}

#endif // __AUTH_CONTROLLER__