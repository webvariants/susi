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


#ifndef __CPPENGINE_BASECONTROLLER__
#define __CPPENGINE_BASECONTROLLER__

#include "events/Event.h"
#include "util/Any.h"
#include "logger/Logger.h"

namespace Susi {
namespace Cpp {


class BaseController {
protected:
	std::shared_ptr<Susi::CPP::Engine> engine;
	void log(Susi::Util::Any any);
	void publish(Susi::Events::EventPtr event, Susi::Events::Consumer finishedCallback = Susi::Events::Consumer{});
	long subscribe(std::string topic, Susi::Events::Consumer c);
	long subscribe(std::string topic, Susi::Events::Processor p);
	void acknowledge(Susi::Events::EventPtr event);
	void unsubscribe(std::string topic);
	Susi::Events::EventPtr createEvent(std::string topic, Susi::Util::Any payload = Susi::Util::Any{});
public:
	BaseController();

	~BaseController();
};


}
}
#endif // __CPPENGINE_BASECONTROLLER__
