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

#ifndef __COMPONENTMANAGER__
#define __COMPONENTMANAGER__

#include "util/Any.h"
#include "world/Component.h"

#include <functional>

namespace Susi {
namespace System {

	class ComponentManager {
	protected:
		typedef std::function<std::shared_ptr<Component>(Susi::Util::Any & config)> RegisterFunction;
		struct ComponentData {
			std::shared_ptr<Component> component{nullptr};
			bool running = false;
		};
		Susi::Util::Any::Object config;
		std::map<std::string,RegisterFunction> registerFunctions;
		std::map<std::string,ComponentData> components;
		std::map<std::string,std::vector<std::string>> dependencies;
		std::map<std::string,std::vector<std::string>> inverseDependencies;
	public:

		ComponentManager(Susi::Util::Any::Object config) : config{config} {}

		void registerComponent(std::string name, RegisterFunction func){
			registerFunctions.emplace(name,std::move(func));
		}
		void registerDependency(std::string subject, std::string dependency){
			auto & deps = dependencies[subject];
			deps.push_back(dependency);
			auto & iDeps = inverseDependencies[dependency];
			iDeps.push_back(subject);
		}

		bool loadComponent(std::string name){
			if(components.find(name) == components.end() && registerFunctions[name]){
				ComponentData data;
				if(config[name].isNull())return false;
				data.component = registerFunctions[name](config[name]);
				components[name] = data;
				return true;
			}
			return false;
		}

		bool unloadComponent(std::string name){
			if(components.find(name) == components.end()){
				return false;
			}
			components[name].component->stop();
			components.erase(name);
			return true;
		}

		bool startComponent(std::string name){
			if(components.find(name) == components.end() && !loadComponent(name)){
				return false;
			}
			auto & data = components[name];
			if(data.running){
				return false;
			}
			for(std::string & dep : dependencies[name]){
				startComponent(dep);
				if(components.find(dep) == components.end() || !components[dep].running){
					return false;
				}
			}
			data.component->start();
			data.running = true;

			return true;
		}
		
		bool stopComponent(std::string name){
			if(components.find(name) == components.end() || components[name].running == false) {
				return false;
			}
			for(std::string & iDep : inverseDependencies[name]){
				stopComponent(iDep);
			}
			components[name].component->stop();
			components[name].running = false;

			return true;
		}
	};
}

}

#endif // __COMPONENTMANAGER__
