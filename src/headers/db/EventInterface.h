#ifndef __DBEVENTINTERFACE__
#define __DBEVENTINTERFACE__

#include "events/EventSystem.h"
#include "db/Database.h"
#include "db/Manager.h"


namespace Susi{
	namespace DB{
		void handleQuery(Susi::Event & event);
		void init();
	}
}


#endif // __DBEVENTINTERFACE__