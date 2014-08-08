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

#ifndef __IO_EVENT_INTERFACE__
#define __IO_EVENT_INTERFACE__


//#include "events/EventSystem.h"
#include "iocontroller/IOController.h"

#include "world/World.h"
#include "events/global.h"


namespace Susi {
	namespace IOEventInterface {
		void handleWriteFile(Susi::Events::EventPtr event);
		
		void handleReadFile(Susi::Events::EventPtr event);

		void handleMovePath(Susi::Events::EventPtr event);
		void handleCopyPath(Susi::Events::EventPtr event);	
		void handleDeletePath(Susi::Events::EventPtr event);

		void handleMakeDir(Susi::Events::EventPtr event);
		void handleSetExecutable(Susi::Events::EventPtr event);
		void handleGetExecutable(Susi::Events::EventPtr event);

		void initEventInterface();
	}
}

#endif // __IO_EVENT_INTERFACE__