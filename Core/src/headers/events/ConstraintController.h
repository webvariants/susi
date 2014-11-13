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

#ifndef __CONSTRAINTCONTROLLER__
#define __CONSTRAINTCONTROLLER__

#include "world/Component.h"

namespace Susi {
namespace Events {

class ConstraintControllerComponent : public Susi::System::Component {
	protected:
	std::shared_ptr<Susi::Events::ManagerComponent> eventManager;
	long subId = -1;

	public:
	ConstraintControllerComponent(Susi::System::ComponentManager * mgr){
		try{
			eventManager = mgr->getComponent<Susi::Events::ManagerComponent>("eventsystem");
		}catch(...){
			LOG(DEBUG) << "cannot setup constraint controller, eventsystem has not the correct type";
		}
	}

	virtual void start() override {
		if(eventManager.get() != nullptr){
			Processor p = [this](EventPtr event){
				std::string left = event->payload["left"];
				std::string right = event->payload["right"];
				eventManager->addConstraint({left,right});
				event->payload["success"] = true;
			};
			subId = eventManager->subscribe(std::string{"constraints::add"},p);
		}
	}	
	
	virtual void stop() override {
		eventManager->unsubscribe(subId);
	}	
};

}
}

#endif // __CONSTRAINTCONTROLLER__
