/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 *
 * @author: Christian Sonderfeld (christian.sonderfeld@webvariants.de)
 */

#ifndef __SESSIONEVENTINTERFACE__
#define __SESSIONEVENTINTERFACE__

#include "events/EventSystem.h"
#include "sessions/SessionManager.h"

namespace Susi {
	namespace Sessions {
		using Susi::Event;
		void handleGetAttribute(Event & event);
		void handleSetAttribute(Event & event);
		void handlePushAttribute(Event & event);
		void handleRemoveAttribute(Event & event);
		void handleUpdate(Event & event);
		void handleCheck(Event & event);
		void initEventInterface();
	}
}

#endif //__SESSIONEVENTINTERFACE__
