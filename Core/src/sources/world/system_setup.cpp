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

	/**
	 * Declare event system
	 */
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

	/**
	 * Declare heartbeat
	 */
	manager->registerComponent("heartbeat",[](ComponentManager * mgr, Any & config){
		return std::shared_ptr<Component>{new Susi::HeartBeatComponent{mgr}};
	});
	manager->registerDependency("heartbeat","eventsystem");

	/**
	 * Declare dbmanager
	 */
	manager->registerComponent("dbmanager", [](ComponentManager * mgr, Any & config) {
		return std::shared_ptr<Component>{new Susi::DB::DBComponent{mgr, config}};
	});
	manager->registerDependency("dbmanager","eventsystem");

	/**
	 * Declare authcontroller
	 */
	manager->registerComponent("authcontroller", [](ComponentManager * mgr, Any & config) {
		std::string db_identifier{"authdb"};
		if(config["db"].isString()){
			db_identifier = static_cast<std::string>(config["db"]);
		}
		return std::shared_ptr<Component>{new Susi::Auth::ControllerComponent{mgr, db_identifier}};
	});
	manager->registerDependency("authcontroller","eventsystem");
	manager->registerDependency("authcontroller","dbmanager");

	/**
	 * TCP Api Server
	 */
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
	manager->registerDependency("tcpapiserver","sessionmanager");

	/**
	 * Declare enginestarter
	 */
	manager->registerComponent("enginestarter", [](ComponentManager * mgr, Any & config) {		
		return std::shared_ptr<Component>{new Susi::EngineStarter::StarterComponent{mgr}};
	});
	manager->registerDependency("enginestarter","eventsystem");

	/**
	 * Declare iocontroller
	 */
	manager->registerComponent("iocontroller", [](ComponentManager * mgr, Any & config) {			
		return std::shared_ptr<Component>{new Susi::IOControllerComponent{mgr}};
	});
	manager->registerDependency("iocontroller","eventsystem");

	/**
	 * Declare sessionmanager
	 */
	manager->registerComponent("sessionmanager", [](ComponentManager * mgr, Any & config) {
		std::chrono::milliseconds lifetime{10000};		
		if(config["lifetime"].isInteger()){
			lifetime =  std::chrono::milliseconds{static_cast<int>(config["lifetime"])};
		}
		return std::shared_ptr<Component>{new Susi::Sessions::SessionManagerComponent{mgr, lifetime}};
	});
	manager->registerDependency("sessionmanager","eventsystem");

	/**
	 * Declare statecontroller
	 */
	manager->registerComponent("statecontroller", [](ComponentManager * mgr, Any & config) {
		std::string file{""};
		if(config["file"].isString()){
			file = static_cast<std::string>(config["file"]);
		}
		return std::shared_ptr<Component>{new Susi::States::StateControllerComponent{mgr, file}};
	});
	manager->registerDependency("statecontroller","eventsystem");
	manager->registerDependency("statecontroller","iocontroller");
	manager->registerDependency("statecontroller","heartbeat");

	/**
	 * Declare syscallcontroller
	 */
	manager->registerComponent("syscallcontroller", [](ComponentManager * mgr, Any & cfg) {
		size_t threads = 4;
		size_t queuelen = 16;
		Any::Object commands = cfg["commands"];
		Any threadsVal = cfg["threads"];
		if(!threadsVal.isNull()){
			threads = (size_t)static_cast<long>(threadsVal);
		}
		Any queuelenVal = cfg["queuelen"];
		if(!queuelenVal.isNull()){
			queuelen = (size_t)static_cast<long>(queuelenVal);
		}
		return std::shared_ptr<Component>{new Susi::Syscall::SyscallComponent{mgr, commands, threads, queuelen}};
	});
	manager->registerDependency("syscallcontroller","eventsystem");
	manager->registerDependency("syscallcontroller","iocontroller");

	/**
	 * Declare httpserver
	 */
	manager->registerComponent("httpserver", [](ComponentManager * mgr, Any & config) {
		std::string address{""};
		if(config["address"].isString()){
			address = static_cast<std::string>(config["address"]);
		}
		std::string assetRoot{""};
		if(config["assets"].isString()){
			assetRoot = static_cast<std::string>(config["assets"]);
		}
		return std::shared_ptr<Component>{new Susi::HttpServerComponent{mgr, address, assetRoot}};
	});
	manager->registerDependency("httpserver","eventsystem");
	manager->registerDependency("httpserver","sessionmanager");
	
	/**
	 * Declare Autodiscovery
	 */
	manager->registerComponent("autodiscovery", [](ComponentManager * mgr, Any & config){
		std::string mcast{"239.23.23.23:4242"};
		std::string ownName{"susi"};
		if(config["mcast"].isString()){
			mcast = static_cast<std::string>(config["mcast"]);
		}
		if(config["address"].isString()){
			ownName = static_cast<std::string>(config["address"]);
		}
		return std::shared_ptr<Component>{new Susi::Autodiscovery::AutoDiscoveryComponent{mcast,ownName,mgr}};
	});
	manager->registerDependency("autodiscovery","eventsystem");

	return manager;
}
