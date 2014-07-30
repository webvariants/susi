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

#ifndef __EVENTSYSTEM__
#define __EVENTSYSTEM__

#include <functional>
#include <Poco/Dynamic/Var.h>
#include <Poco/JSON/Stringifier.h>
#include <Poco/JSON/Object.h>
#include <map>
#include <vector>
#include <sstream>
#include <Poco/Dynamic/Struct.h>
#include <Poco/Timestamp.h>
#include <Poco/Glob.h>
#include <memory>
#include <mutex>
#include <condition_variable>

#include "events/Event.h"

namespace Susi {

class EventSystem {
protected:
	std::map<std::string,std::map<int,std::function<void(Event&)>>> callbacks;
	std::map<std::string,std::map<int,std::function<void(Event&)>>> globs;
	std::mutex mutex;
public:
	long subscribe(std::string topic,std::function<void(Event&)> callback);
	void unsubscribe(std::string topic,long id);
	long subscribe_glob(std::string topic,std::function<void(Event&)> callback);
	void unsubscribe_glob(std::string topic,long id);
	void publish(Event & event);
};

/**
 * Basic API
 */
long subscribe(std::string topic,std::function<void(Event&)> callback);
void publish(Event & event);
void publish(Event & event, std::function<void(Susi::Event&)> callback);
void unsubscribe(std::string topic,long id);

/**
 * Api Helper
 */
void answerEvent(Event &request, Poco::Dynamic::Var response);
void once(std::string topic,std::function<void(Event&)> callback);
Poco::Dynamic::Var request(std::string topic,Poco::Dynamic::Var payload,std::chrono::duration<int,std::milli> timeout);
Poco::Dynamic::Var request(std::string topic);

}

#endif // __EVENTSYSTEM__
