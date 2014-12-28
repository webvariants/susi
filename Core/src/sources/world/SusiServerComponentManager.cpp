#include "world/SusiServerComponentManager.h"

Susi::System::SusiServerComponentManager::SusiServerComponentManager(Susi::Util::Any::Object config) : Susi::System::ComponentManager{config} {
	
	using Susi::System::ComponentManager;
	using Susi::System::Component;
	using Susi::Util::Any;

	/**
	 * Declare logger
	 */
	registerComponent("logger",[](ComponentManager * mgr, Any & config){
		std::string configFile;
		if(config["easylogging++"].isString()){
			configFile = static_cast<std::string>(config["easylogging++"]);
		}
		return std::shared_ptr<Component>{new Susi::LoggerComponent{configFile}};
	});

	/**
	 * Declare event system
	 */
	registerComponent("eventsystem",[](ComponentManager * mgr, Any & config){
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
	registerDependency("eventsystem","logger");

	/**
	 * Declare heartbeat
	 */
	registerComponent("heartbeat",[](ComponentManager * mgr, Any & config){
		return std::shared_ptr<Component>{new Susi::HeartBeatComponent{mgr}};
	});
	registerDependency("heartbeat","eventsystem");

	/**
	 * Declare dbmanager
	 */
	registerComponent("dbmanager", [](ComponentManager * mgr, Any & config) {
		return std::shared_ptr<Component>{new Susi::DB::DBComponent{mgr, config}};
	});
	registerDependency("dbmanager","eventsystem");
	registerDependency("dbmanager","sessionmanager");

	/**
	 * Declare authcontroller
	 */
	registerComponent("authcontroller", [](ComponentManager * mgr, Any & config) {
		std::string db_identifier{"authdb"};
		if(config["db"].isString()){
			db_identifier = static_cast<std::string>(config["db"]);
		}
		return std::shared_ptr<Component>{new Susi::Auth::ControllerComponent{mgr, db_identifier}};
	});
	registerDependency("authcontroller","eventsystem");
	registerDependency("authcontroller","dbmanager");
	registerDependency("authcontroller","sessionmanager");

	/**
	 * TCP Api Server
	 */
	registerComponent("tcpapiserver", [](ComponentManager * mgr, Any & config) {
		std::string address{""};
		if(config["address"].isString()){
			address = static_cast<std::string>(config["address"]);
		}
		return std::shared_ptr<Component>{new Susi::Api::TCPApiServerComponent{mgr, address}};
	});
	registerDependency("tcpapiserver","apiserver");

	/**
	 * Declare enginestarter
	 */
	registerComponent("enginestarter", [](ComponentManager * mgr, Any & config) {		
		return std::shared_ptr<Component>{new Susi::EngineStarter::StarterComponent{mgr}};
	});
	registerDependency("enginestarter","eventsystem");
	registerDependency("enginestarter","sessionmanager");

	/**
	 * Declare iocontroller
	 */
	registerComponent("iocontroller", [](ComponentManager * mgr, Any & config) {			
		return std::shared_ptr<Component>{new Susi::IOControllerComponent{mgr}};
	});
	registerDependency("iocontroller","eventsystem");
	registerDependency("iocontroller","sessionmanager");

	/**
	 * Declare sessionmanager
	 */
	registerComponent("sessionmanager", [](ComponentManager * mgr, Any & config) {
		std::chrono::milliseconds lifetime{10000};		
		if(config["lifetime"].isInteger()){
			lifetime =  std::chrono::milliseconds{static_cast<int>(config["lifetime"])};
		}
		return std::shared_ptr<Component>{new Susi::Sessions::SessionManagerComponent{mgr, lifetime}};
	});
	registerDependency("sessionmanager","eventsystem");
	registerDependency("sessionmanager","heartbeat");
	/**
	 * Declare statecontroller
	 */
	registerComponent("statecontroller", [](ComponentManager * mgr, Any & config) {
		std::string file{""};
		if(config["file"].isString()){
			file = static_cast<std::string>(config["file"]);
		}
		return std::shared_ptr<Component>{new Susi::States::StateControllerComponent{mgr, file}};
	});
	registerDependency("statecontroller","eventsystem");
	registerDependency("statecontroller","iocontroller");
	registerDependency("statecontroller","heartbeat");
	registerDependency("statecontroller","sessionmanager");

	/**
	 * Declare syscallcontroller
	 */
	registerComponent("syscallcontroller", [](ComponentManager * mgr, Any & cfg) {
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
	registerDependency("syscallcontroller","eventsystem");
	registerDependency("syscallcontroller","iocontroller");
	registerDependency("syscallcontroller","sessionmanager");

	/**
	 * Declare httpserver
	 */
	registerComponent("httpserver", [](ComponentManager * mgr, Any & config) {
		std::string host="0.0.0.0",port="8080",assetRoot="./",uploadRoot="./";

		if(config["host"].isString()){
			host = static_cast<std::string>(config["host"]);
		}
		if(config["assets"].isString()){
			assetRoot = static_cast<std::string>(config["assets"]);
		}
		if(config["upload"].isString()){
			uploadRoot = static_cast<std::string>(config["upload"]);
		}

		return std::shared_ptr<Component>{new Susi::Webstack::HttpServerComponent{mgr, host, port, assetRoot, uploadRoot}};
	});	
	registerDependency("httpserver","apiserver");
	registerDependency("httpserver","sessionmanager");
	
	/**
	 * Declare Autodiscovery
	 */
	registerComponent("autodiscovery", [](ComponentManager * mgr, Any & config){
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
	registerDependency("autodiscovery","eventsystem");
	registerDependency("autodiscovery","sessionmanager");

	/**
	 * Declare apiserver
	 */
	registerComponent("apiserver", [](ComponentManager * mgr, Any & config) {			
		return std::shared_ptr<Component>{new Susi::Api::ApiServerComponent{mgr}};
	});
	registerDependency("apiserver","eventsystem");
	registerDependency("apiserver","sessionmanager");

	/**
	 * Declare constraints
	 */
	registerComponent("constraints", [](ComponentManager * mgr, Any & config) {			
		return std::shared_ptr<Component>{new Susi::Events::ConstraintControllerComponent{mgr}};
	});
	registerDependency("constraints","eventsystem");
	registerDependency("constraints","sessionmanager");

	/**
	 * Declare DDHCP
	 */
	registerComponent("ddhcp", [](ComponentManager * mgr, Any & config) {			
		unsigned short port = 1704;
		try{
			port = (unsigned short)(int)config[port];
		}catch(...){}
		return std::shared_ptr<Component>{new Susi::Ddhcp::DDHCPComponent{mgr,port}};
	});
	registerDependency("ddhcp","eventsystem");
	registerDependency("ddhcp","sessionmanager");

	/**
	 * Declare duktape js engine
	 */
	registerComponent("duktape",[](ComponentManager * mgr, Any & config){
		std::string source = "";
		try{
			source = static_cast<std::string>(config["source"]);
		}catch(...){}
		return std::shared_ptr<Component>{new Susi::Duktape::JSEngine{mgr,source}};
	});
}
