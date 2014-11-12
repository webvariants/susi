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

std::shared_ptr<Susi::System::ComponentManager> Susi::System::createSusiComponentManager(Susi::Util::Any::Object config) {
	using Susi::System::ComponentManager;
	using Susi::System::Component;
	using Susi::Util::Any;

	auto manager = std::make_shared<Susi::System::ComponentManager>(config);

	/**
	 * Declare logger
	 */
	manager->registerComponent("logger",[](ComponentManager * mgr, Any & config){
		std::string configFile;
		if(config["easylogging++"].isString()){
			configFile = static_cast<std::string>(config["easylogging++"]);
		}
		return std::shared_ptr<Component>{new Susi::LoggerComponent{configFile}};
	});

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
	manager->registerDependency("eventsystem","logger");

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
	manager->registerDependency("dbmanager","sessionmanager");

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
	manager->registerDependency("authcontroller","sessionmanager");

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
	manager->registerDependency("tcpapiserver","apiserver");

	/**
	 * Declare enginestarter
	 */
	manager->registerComponent("enginestarter", [](ComponentManager * mgr, Any & config) {		
		return std::shared_ptr<Component>{new Susi::EngineStarter::StarterComponent{mgr}};
	});
	manager->registerDependency("enginestarter","eventsystem");
	manager->registerDependency("enginestarter","sessionmanager");

	/**
	 * Declare iocontroller
	 */
	manager->registerComponent("iocontroller", [](ComponentManager * mgr, Any & config) {			
		return std::shared_ptr<Component>{new Susi::IOControllerComponent{mgr}};
	});
	manager->registerDependency("iocontroller","eventsystem");
	manager->registerDependency("iocontroller","sessionmanager");

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
	manager->registerDependency("sessionmanager","heartbeat");
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
	manager->registerDependency("statecontroller","sessionmanager");

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
	manager->registerDependency("syscallcontroller","sessionmanager");

	/**
	 * Declare httpserver
	 */
	manager->registerComponent("httpserver", [](ComponentManager * mgr, Any & config) {
		std::string address{""};
		size_t threads{4};
		size_t backlog{16};

		if(config["address"].isString()){
			address = static_cast<std::string>(config["address"]);
		}
		std::string assetRoot{""};
		if(config["assets"].isString()){
			assetRoot = static_cast<std::string>(config["assets"]);
		}
		std::string upload{""};
		if(config["upload"].isString()){
			upload = static_cast<std::string>(config["upload"]);
		}

		if(config["threads"].isInteger()){
			threads =  static_cast<long>(config["threads"]);
		}
		if(config["backlog"].isInteger()){
			backlog =  static_cast<long>(config["backlog"]);
		}
		return std::shared_ptr<Component>{new Susi::HttpServerComponent{mgr, address, assetRoot, upload, threads, backlog}};
	});	
	manager->registerDependency("httpserver","apiserver");
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
	manager->registerDependency("autodiscovery","sessionmanager");

	/**
	 * Declare apiserver
	 */
	manager->registerComponent("apiserver", [](ComponentManager * mgr, Any & config) {			
		return std::shared_ptr<Component>{new Susi::Api::ApiServerComponent{mgr}};
	});
	manager->registerDependency("apiserver","eventsystem");
	manager->registerDependency("apiserver","sessionmanager");

	/**
	 * Declare constraints
	 */
	manager->registerComponent("constraints", [](ComponentManager * mgr, Any & config) {			
		return std::shared_ptr<Component>{new Susi::Events::ConstraintControllerComponent{mgr}};
	});
	manager->registerDependency("constraints","eventsystem");
	manager->registerDependency("constraints","sessionmanager");

	/**
	 * Declare DDHCP
	 */
	manager->registerComponent("ddhcp", [](ComponentManager * mgr, Any & config) {			
		unsigned short port = 1704;
		try{
			port = (unsigned short)(int)config[port];
		}catch(...){}
		return std::shared_ptr<Component>{new Susi::Ddhcp::DDHCPComponent{mgr,port}};
	});
	manager->registerDependency("ddhcp","eventsystem");
	manager->registerDependency("ddhcp","sessionmanager");

	return manager;
}
