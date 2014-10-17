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


#ifndef __DEBUGGER_PRINT_CONTROLLER__
#define __DEBUGGER_PRINT_CONTROLLER__

#include <atomic>
#include "engine/BaseController.h"

namespace Debugger {

class PrintController : public Susi::Cpp::BaseController {
protected:
	std::string topic;
	std::atomic<int> *times;
	std::condition_variable *cond;
public:
	PrintController(std::string _topic, std::atomic<int> *_times, std::condition_variable *_cond) : topic{_topic} {
		times = _times;
		cond = _cond;
	};

	virtual void start() override {
		Susi::Events::Consumer c = [this](Susi::Events::SharedEventPtr event){
			std::cout<<event->toString()<<std::endl;
			// -1 means no break
			if(times->load() > -1) {
				times->store(times->load() -1);
				if(times->load() == 0) {
					cond->notify_one();
				}
			}
		};
		
		subscribe(topic, c);		
	}
};

}

#endif // __DEBUGGER_PRINT_CONTROLLER__
