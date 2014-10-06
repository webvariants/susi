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

#include "world/BaseComponent.h"

namespace Susi {
namespace Events {

class ConstraintControllerComponent : public Susi::System::BaseComponent {
	public:
	ConstraintControllerComponent(Susi::System::ComponentManager * mgr) : Susi::System::BaseComponent{mgr} {}

	virtual void start() override {
		Processor p = [this](EventPtr event){
			std::string left = event->payload["left"];
			std::string right = event->payload["right"];
			eventManager->addConstraint({left,right});
			event->payload["success"] = true;
		};
		subscribe(std::string{"constraints::add"},p);
	}	
	
	virtual void stop() override {
		unsubscribeAll();	
	}	
};

}
}

#endif // __CONSTRAINTCONTROLLER__
