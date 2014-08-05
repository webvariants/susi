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

#ifndef __STATE_EVENT_INTERFACE__
#define __STATE_EVENT_INTERFACE__


#include "events/EventSystem.h"
#include "states/StateController.h"

#include "world/World.h"
#include "events/global.h"


namespace Susi {
	namespace States {
		namespace EventInterface {	
			void init();

			void handleSetState(Susi::Events::EventPtr event);	
			void handleGetState(Susi::Events::EventPtr event);

			void handleSetPersistentState(Susi::Events::EventPtr event);
			void handleGetPersistentState(Susi::Events::EventPtr event);		
		}
	}
}

#endif // __STATE_EVENT_INTERFACE__