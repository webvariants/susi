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

#include "world/ComponentManager.h"
#include "util/Any.h"
#include "events/ManagerComponent.h"

std::shared_ptr<Susi::System::ComponentManager> createSusiComponentManager(Susi::Util::Any::Object config){
	using Susi::System::ComponentManager;
	using Susi::System::Component;
	using Susi::Util::Any;

	auto manager = std::make_shared<Susi::System::ComponentManager>(config);
	
	manager->registerComponent("event-system",[](ComponentManager * mgr, Any & config){
		size_t threads = 4;
		size_t queuelen = 32;
		if(config["threads"].isInteger()){
			threads = static_cast<long>(config["threads"]);
		}
		if(config["queuelen"].isInteger()){
			queuelen = static_cast<long>(config["queuelen"]);
		}
		return std::shared_ptr<Component>{new Susi::Events::ManagerComponent{threads,queuelen}};
	});

	manager->registerComponent("heartbeat",[](ComponentManager * mgr, Any & config){
		return std::shared_ptr<Component>{new Susi::HeartBeatComponent{mgr}};
	});

	manager->registerDependency("heartbeat","event-system");

	return manager;
}