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

#include "world/system_setup.h"

std::shared_ptr<Susi::System::ComponentManager> Susi::System::createSusiComponentManager(Susi::Util::Any::Object config){
	using Susi::System::ComponentManager;
	using Susi::System::Component;
	using Susi::Util::Any;

	auto manager = std::make_shared<Susi::System::ComponentManager>(config);

	manager->registerComponent("eventsystem",[](ComponentManager * mgr, Any & config){
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

	manager->registerDependency("heartbeat","eventsystem");

	manager->registerComponent("dbmanager", [](ComponentManager * mgr, Any & config) {
		return std::shared_ptr<Component>{new Susi::DB::DBComponent{mgr, config}};
	});

	manager->registerDependency("dbmanager","eventsystem");

	manager->registerComponent("authcontroller", [](ComponentManager * mgr, Any & config) {
		std::string db_identifier{""};
		if(config["db_identifier"].isString()){
			db_identifier = static_cast<std::string>(config["db_identifier"]);
		}
		return std::shared_ptr<Component>{new Susi::Auth::ControllerComponent{mgr, db_identifier}};
	});

	manager->registerDependency("authcontroller","eventsystem");
	manager->registerDependency("heartbeat","dbmanager");

	manager->registerComponent("tcpapiserver", [](ComponentManager * mgr, Any & config) {
		std::string address{""};
		size_t threads{4};
		size_t backlog{16};
		if(config["address"].isString()){
			address = static_cast<std::string>(config["address"]);
		}
		if(config["threads"].isInteger()){
			threads =  static_cast<long>(config["threads"]);
		}
		if(config["backlog"].isInteger()){
			backlog =  static_cast<long>(config["backlog"]);
		}
		return std::shared_ptr<Component>{new Susi::Api::TCPApiServerComponent{mgr, address, threads, backlog}};
	});

	manager->registerDependency("tcpapiserver","eventsystem");

	manager->registerComponent("enginestarter", [](ComponentManager * mgr, Any & config) {
		std::string path{""};
		if(config["path"].isString()){
			path = static_cast<std::string>(config["path"]);
		}
		return std::shared_ptr<Component>{new Susi::EngineStarter::StarterComponent{mgr, path}};
	});

	manager->registerDependency("enginestarter","eventsystem");

	manager->registerComponent("iocontroller", [](ComponentManager * mgr, Any & config) {
		std::string base_path{""};
		if(config["base_path"].isString()){
			base_path = static_cast<std::string>(config["base_path"]);
		}
		return std::shared_ptr<Component>{new Susi::IOControllerComponent{mgr, base_path}};
	});

	manager->registerDependency("iocontroller","eventsystem");

	manager->registerComponent("sessionmanager", [](ComponentManager * mgr, Any & config) {
		std::chrono::milliseconds stdSessionLifetime{10000};
		std::chrono::milliseconds checkInterval{1000};
		if(config["stdSessionLifetime"].isInteger()){
			stdSessionLifetime =  std::chrono::milliseconds{static_cast<int>(config["stdSessionLifetime"])};
		}
		if(config["checkInterval"].isInteger()){
			checkInterval =  std::chrono::milliseconds{static_cast<int>(config["checkInterval"])};
		}
		return std::shared_ptr<Component>{new Susi::Sessions::SessionManagerComponent{mgr, stdSessionLifetime, checkInterval}};
	});

	manager->registerDependency("sessionmanager","eventsystem");

	manager->registerComponent("statecontroller", [](ComponentManager * mgr, Any & config) {
		std::string file{""};
		if(config["file"].isString()){
			file = static_cast<std::string>(config["file"]);
		}
		return std::shared_ptr<Component>{new Susi::States::StateControllerComponent{mgr, file}};
	});

	manager->registerDependency("statecontroller","eventsystem");
	manager->registerDependency("statecontroller","iocontroller");

	manager->registerComponent("syscallcontroller", [](ComponentManager * mgr, Any & config) {
		std::string config_path{""};
		if(config["config_path"].isString()){
			config_path = static_cast<std::string>(config["config_path"]);
		}
		return std::shared_ptr<Component>{new Susi::Syscall::SyscallControllerComponent{mgr, config_path}};
	});

	manager->registerDependency("syscallcontroller","eventsystem");
	manager->registerDependency("syscallcontroller","iocontroller");

	manager->registerComponent("httpserver", [](ComponentManager * mgr, Any & config) {
		std::string address{""};
		if(config["address"].isString()){
			address = static_cast<std::string>(config["address"]);
		}
		std::string assetRoot{""};
		if(config["assetRoot"].isString()){
			assetRoot = static_cast<std::string>(config["assetRoot"]);
		}
		return std::shared_ptr<Component>{new Susi::HttpServerComponent{mgr, address, assetRoot}};
	});

	manager->registerDependency("httpserver","eventsystem");
	manager->registerDependency("httpserver","tcpapiserver");

	return manager;
}
