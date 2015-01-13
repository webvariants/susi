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

#ifndef __PLUGINLOADINGCOMPONENTMANAGER__
#define __PLUGINLOADINGCOMPONENTMANAGER__

#include "world/ComponentManager.h"
#include <Poco/Glob.h>
#include <Poco/SharedLibrary.h>

namespace Susi {
	namespace System {

		typedef std::shared_ptr<Susi::System::Component>(*CreateComponent_t)(Susi::System::ComponentManager*,Susi::Util::Any&); 
		typedef std::string (*GetName_t)();
		typedef std::vector<std::string>(*GetDependencies_t)(); 

		class PluginLoadingComponentManager : public ComponentManager {
		protected:
			const std::string symbol_getName = "getName";
			const std::string symbol_createComponent = "createComponent";
			const std::string symbol_getDependencies = "getDependencies";

			std::vector<std::shared_ptr<Poco::SharedLibrary>> libs;

		public:
			PluginLoadingComponentManager(Susi::Util::Any::Object config) : ComponentManager{config} {
				if(config["plugins"].isObject() && config["plugins"]["path"].isString()){
					std::string path = config["plugins"]["path"];
					LOG(DEBUG) << "got plugin path " << path;
					std::string pattern = path+"/*"+Poco::SharedLibrary::suffix();
					LOG(DEBUG) << "pattern " << pattern;
					std::set<std::string> sharedLibraryFiles;
		            Poco::Glob::glob(pattern, sharedLibraryFiles);
		            LOG(DEBUG) << "shared libs: "<<sharedLibraryFiles.size();
		            for(auto & path : sharedLibraryFiles){
		                libs.emplace_back(std::make_shared<Poco::SharedLibrary>(path));
		                auto lib = libs[libs.size()-1];
		                if(lib->hasSymbol(symbol_getName) && 
		                   lib->hasSymbol(symbol_createComponent)){
		                	LOG(INFO) << "found lib " << path;
		                    GetName_t getName = (GetName_t)lib->getSymbol(symbol_getName);
		                    CreateComponent_t createComponent = (CreateComponent_t)lib->getSymbol(symbol_createComponent);
		                    std::string name = getName();
		                    registerComponent(name,createComponent);
		                    if(lib->hasSymbol(symbol_getDependencies)){
		                    	GetDependencies_t getDependencies = (GetDependencies_t)lib->getSymbol(symbol_getDependencies);
		                    	auto deps = getDependencies();
		                    	for(std::string & dep : deps){
		                    		registerDependency(name, dep);
		                    	}
		                    }
		                }
		            }
				}
			}
		};
	}
}

#endif // __PLUGINLOADINGCOMPONENTMANAGER__
