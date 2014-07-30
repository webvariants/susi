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

#ifndef __AUTH_EVENT_INTERFACE__
#define __AUTH_EVENT_INTERFACE__

#include "events/EventSystem.h"
#include "auth/AuthController.h"


namespace Susi {
	namespace Auth {
		namespace EventInterface {
			void handleLogin(Susi::Event & event);
			void handleLogout(Susi::Event & event);
			void handleIsLoggedIn(Susi::Event & event);
			void handleGetUsername(Susi::Event & event);

			void init();
		}
	}
}


#endif // __AUTH_EVENT_INTERFACE__