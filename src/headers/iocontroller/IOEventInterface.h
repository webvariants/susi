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

#include "world/World.h"
#include "events/global.h"

namespace Susi {
	namespace IOEventInterface {
		void handleWriteFile(Susi::Events::EventPtr event);
		
		void handleReadFile(Susi::Event & event);

		void handleMovePath(Susi::Event & event);
		void handleCopyPath(Susi::Event & event);	
		void handleDeletePath(Susi::Event & event);

		void handleMakeDir(Susi::Event & event);
		void handleSetExecutable(Susi::Event & event);
		void handleGetExecutable(Susi::Event & event);

		void initEventInterface();
	}
}

#endif // __IO_EVENT_INTERFACE__