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

#ifndef __ENGINE_EVENT_INTERFACE__
#define __ENGINE_EVENT_INTERFACE__

#include "enginestarter/EngineStarter.h"
#include "world/World.h"
//#include "events/global.h"


namespace Susi {
	namespace EngineStarter {
		namespace EventInterface {	
			void init();

			void handleStart(Susi::Events::EventPtr event);						
			void handleRestart(Susi::Events::EventPtr event);
			void handleStop(Susi::Events::EventPtr event);
		}
	}
}

#endif // __ENGINE_EVENT_INTERFACE__