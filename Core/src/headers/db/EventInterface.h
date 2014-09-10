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

#ifndef __DB_EVENT_INTERFACE__
#define __DB_EVENT_INTERFACE__

#include "db/Database.h"
#include "db/Manager.h"

#include "world/World.h"
#include "events/global.h"

namespace Susi{
	namespace DB{
		void handleQuery(Susi::Events::EventPtr event);
		void init();
	}
}


#endif // __DB_EVENT_INTERFACE__