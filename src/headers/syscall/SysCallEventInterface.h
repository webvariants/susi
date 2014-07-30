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

#ifndef __SYS_CALL_EVENT_INTERFACE__
#define __SYS_CALL_EVENT_INTERFACE__

#include "events/EventSystem.h"
#include "syscall/SysCallController.h"

namespace Susi {
	namespace Syscall {
		namespace EventInterface {
			void handleStartProcess(Susi::Event & event);
			void init();
		}
	}
}

#endif // __SYS_CALL_EVENT_INTERFACE__