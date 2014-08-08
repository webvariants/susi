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

#ifndef __SESSIONEVENTINTERFACE__
#define __SESSIONEVENTINTERFACE__

#include "events/EventSystem.h"
#include "sessions/SessionManager.h"

#include "world/World.h"
#include "events/global.h"

namespace Susi {
	namespace Sessions {
		using Susi::Event;
		void handleGetAttribute(Susi::Events::EventPtr event);
		void handleSetAttribute(Susi::Events::EventPtr event);
		void handlePushAttribute(Susi::Events::EventPtr event);
		void handleRemoveAttribute(Susi::Events::EventPtr event);
		void handleUpdate(Susi::Events::EventPtr event);
		void handleCheck(Susi::Events::EventPtr event);
		void initEventInterface();
	}
}

#endif //__SESSIONEVENTINTERFACE__
