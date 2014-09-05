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
		typedef std::function<std::shared_ptr<Component>(ComponentManager * mgr, Susi::Util::Any & config)> RegisterFunction;
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

		void registerComponent(std::string name, RegisterFunction func);
		void registerDependency(std::string subject, std::string dependency);
		bool loadComponent(std::string name);
		bool unloadComponent(std::string name);
		bool startComponent(std::string name);
		bool stopComponent(std::string name);


		bool startAll();
		bool stopAll();

		template<class T=Component>
		std::shared_ptr<T> getComponent(std::string name){
			if(components.find(name) == components.end() && !loadComponent(name)){
				return std::shared_ptr<T>{};
			}
			return std::dynamic_pointer_cast<T>(components[name].component);
		}
	};
}

}

#endif // __COMPONENTMANAGER__
