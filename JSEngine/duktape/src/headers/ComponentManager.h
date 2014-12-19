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

#ifndef __SAMPLECOMPONENTMANAGER__
#define __SAMPLECOMPONENTMANAGER__

#include "world/ComponentManager.h"
#include "apiserver/ApiClient.h"
#include "logger/Logger.h"

#include "DuktapeEngine.h"

namespace Duktape {

class ComponentManager : public Susi::System::ComponentManager {
public:
	ComponentManager(Susi::Util::Any::Object config) : Susi::System::ComponentManager{config} {
		using Susi::Util::Any;
		using Susi::System::Component;
		/**
		 * Declare logger
		 */
		registerComponent("logger",[](Susi::System::ComponentManager * mgr, Any & config){
			std::string configFile;
			if(config["easylogging++"].isString()){
				configFile = static_cast<std::string>(config["easylogging++"]);
			}
			return std::shared_ptr<Component>{new Susi::LoggerComponent{configFile}};
		});

		/**
		 * Declare event system
		 */
		registerComponent("eventsystem",[](Susi::System::ComponentManager * mgr, Any & config){
			std::string addr = "";
			if(config["addr"].isString()){
				addr = static_cast<std::string>(config["addr"]);
			}
			auto ptr = new Susi::Api::ApiClientComponent{addr};
			auto component = std::shared_ptr<Component>{ptr};
			return component;
		});
		registerDependency("eventsystem","logger");

		/**
		 * Declare Sampleconnector
		 */
		registerComponent("duktape",[](Susi::System::ComponentManager * mgr, Any & config){
			std::string src = "";
			if(config["src"].isString()){
				src = static_cast<std::string>(config["src"]);
			}
			bool interactive = false;
			if(config["interactive"].isBool()){
				interactive = static_cast<bool>(config["interactive"]);
			}
			return std::shared_ptr<Component>{new JSEngine{mgr,src,interactive}};
		});

	}
};

}

#endif // __SAMPLECOMPONENTMANAGER__
