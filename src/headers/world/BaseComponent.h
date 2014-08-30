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

#ifndef __BASECOMPONENT__
#define __BASECOMPONENT__

#include "world/Component.h"
#include "world/ComponentManager.h"
#include "events/Manager.h"

namespace Susi {
namespace System {

class BaseComponent : public Component {
protected:
	ComponentManager * componentManager;
	std::shared_ptr<Susi::Events::Manager> eventManager;
public:
	BaseComponent(ComponentManager * manager) : componentManager{manager} {
		eventManager = componentManager->getComponent<Susi::Events::Manager>("eventmanager");
	}
	long subscribe(std::string topic, Susi::Events::Processor processor){
		return eventManager->subscribe(topic,std::move(processor));
	}
	long subscribe(Susi::Events::Predicate pred, Susi::Events::Processor processor){
		return eventManager->subscribe(pred,std::move(processor));
	}
	long subscribe(std::string topic, Susi::Events::Consumer consumer){
		return eventManager->subscribe(topic,std::move(consumer));
	}
	long subscribe(Susi::Events::Predicate pred, Susi::Events::Consumer consumer){
		return eventManager->subscribe(pred,std::move(consumer));
	}
	bool unsubscribe(long id){
		return eventManager->unsubscribe(id);
	}
	void publish(Susi::Events::EventPtr event, Susi::Events::Consumer finishCallback = Susi::Events::Consumer{}){
		eventManager->publish(std::move(event),finishCallback);
	}
	Susi::Events::EventPtr createEvent(std::string topic){
		return eventManager->createEvent(topic);
	}
};

}

#endif // __BASECOMPONENT__
