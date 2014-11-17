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

#ifndef __SYSTEMCONNECTOR__
#define __SYSTEMCONNECTOR__


#include "world/BaseComponent.h"
#include <sstream>

namespace SampleNamespace {

class SampleConnector : public Susi::System::BaseComponent {
public:
	SampleConnector(Susi::System::ComponentManager *mgr) : Susi::System::BaseComponent{mgr} {}

	// The start function subscribes to events and attaches its handlers
	virtual void start() override {
		subscribe(std::string{"system::getTime"},Susi::Events::Processor{[this](Susi::Events::EventPtr event){
			handleGetTime(std::move(event));
		}});
	}

	// When stopping, all subscriptions are deleted
	virtual void stop() override {
		unsubscribeAll();
	}

protected:
	// These are the actual event handlers
	void handleGetTime(Susi::Events::EventPtr event);

};

}

#endif // __SYSTEMCONNECTOR__
