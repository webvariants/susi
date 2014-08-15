/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 * 
 * @author: Tino Rusch (tino.rusch@webvariants.de)
 */

#ifndef __EVENTSGLOBAL__
#define __EVENTSGLOBAL__

#include "events/Manager.h"
#include "events/Event.h"
 
namespace Susi {
	namespace Events {
		long subscribe(std::string topic, Processor handler);
		long subscribe(Predicate pred, Processor handler);
		long subscribe(std::string topic, Consumer handler);
		long subscribe(Predicate pred, Consumer handler);
		bool unsubscribe(long id);
		void publish(EventPtr event, Consumer finishCallback = Consumer{});
		void ack(EventPtr event);
		void deleter(Event *event);
		EventPtr createEvent(std::string topic);

		/*EventPtr fromAny(Susi::Util::Any & val);
		Susi::Util::Any toAny(EventPtr);*/

	}	
}



#endif // __EVENTSGLOBAL__