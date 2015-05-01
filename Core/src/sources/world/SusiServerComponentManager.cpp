#include "susi/world/SusiServerComponentManager.h"

Susi::System::SusiServerComponentManager::SusiServerComponentManager(BSON::Object config) : Susi::System::PluginLoadingComponentManager{config} {
	
	using Susi::System::ComponentManager;
	using Susi::System::Component;
	using BSON::Value;

	/**
	 * Declare logger
	 */
	registerComponent("logger",[](ComponentManager * mgr, BSON::Value& config){
		std::string configFile;
		if(config["easylogging++"].isString()){
			configFile = config["easylogging++"].getString();
		}
		bool syslog = false;
		if(config["syslog"].isBool()){
			syslog = static_cast<bool>(config["syslog"]);
		}
		return std::shared_ptr<Component>{new Susi::LoggerComponent{configFile,syslog}};
	});

	/**
	 * Declare event system
	 */
	registerComponent("eventsystem",[](ComponentManager * mgr, BSON::Value& config){
		size_t threads = 4;
		size_t queuelen = 32;
		if(config["threads"].isInteger()){
			threads = config["threads"].getInt64();
		}
		if(config["queuelen"].isInteger()){
			queuelen = config["queuelen"].getInt64();
		}
		return std::shared_ptr<Component>{new Susi::Events::ManagerComponent{threads,queuelen}};
	});
	registerDependency("eventsystem","logger");

	/**
	 * Declare heartbeat
	 */
	registerComponent("heartbeat",[](ComponentManager * mgr, BSON::Value& config){
		return std::shared_ptr<Component>{new Susi::HeartBeatComponent{mgr}};
	});
	registerDependency("heartbeat","eventsystem");

	/**
	 * Declare dbmanager
	 */
	registerComponent("dbmanager", [](ComponentManager * mgr, BSON::Value& config) {
		return std::shared_ptr<Component>{new Susi::DB::DBComponent{mgr, config}};
	});
	registerDependency("dbmanager","eventsystem");
	registerDependency("dbmanager","sessionmanager");

	/**
	 * Declare authcontroller
	 */
	registerComponent("authcontroller", [](ComponentManager * mgr, BSON::Value& config) {
		std::string db_identifier{"authdb"};
		if(config["db"].isString()){
			db_identifier = config["db"].getString();
		}
		return std::shared_ptr<Component>{new Susi::Auth::ControllerComponent{mgr, db_identifier}};
	});
	registerDependency("authcontroller","eventsystem");
	registerDependency("authcontroller","dbmanager");
	registerDependency("authcontroller","sessionmanager");

	/**
	 * TCP Api Server
	 */
	registerComponent("tcpapiserver", [](ComponentManager * mgr, BSON::Value& config) {
		std::string address{""};
		if(config["address"].isString()){
			address = config["address"].getString();
		}
		return std::shared_ptr<Component>{new Susi::Api::TCPApiServerComponent{mgr, address}};
	});
	registerDependency("tcpapiserver","apiserver");

	/**
	 * Declare enginestarter
	 */
	registerComponent("enginestarter", [](ComponentManager * mgr, BSON::Value& config) {	
		std::string path{""};
		if(config["path"].isString()){
			path = config["path"].getString();
		}
		return std::shared_ptr<Component>{new Susi::EngineStarter::StarterComponent{mgr,path}};
	});
	registerDependency("enginestarter","eventsystem");
	registerDependency("enginestarter","sessionmanager");

	/**
	 * Declare iocontroller
	 */
	registerComponent("iocontroller", [](ComponentManager * mgr, BSON::Value& config) {			
		return std::shared_ptr<Component>{new Susi::IOControllerComponent{mgr}};
	});
	registerDependency("iocontroller","eventsystem");
	registerDependency("iocontroller","sessionmanager");

	/**
	 * Declare sessionmanager
	 */
	registerComponent("sessionmanager", [](ComponentManager * mgr, BSON::Value& config) {
		std::chrono::milliseconds lifetime{10000};		
		if(config["lifetime"].isInteger()){
			lifetime =  std::chrono::milliseconds{config["lifetime"].getInt64()};
		}
		return std::shared_ptr<Component>{new Susi::Sessions::SessionManagerComponent{mgr, lifetime}};
	});
	registerDependency("sessionmanager","eventsystem");
	registerDependency("sessionmanager","heartbeat");
	
	/**
	 * Declare statecontroller
	 */
	registerComponent("statecontroller", [](ComponentManager * mgr, BSON::Value& config) {
		std::string file{""};
		if(config["file"].isString()){
			file = config["file"].getString();
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
	registerComponent("syscallcontroller", [](ComponentManager * mgr, BSON::Value& cfg) {
		size_t threads = 4;
		size_t queuelen = 16;
		BSON::Object commands = cfg["commands"];
		BSON::Value threadsVal = cfg["threads"];
		if(!threadsVal.isUndefined()){
			threads = (size_t)threadsVal.getInt64();
		}
		BSON::Value queuelenVal = cfg["queuelen"];
		if(!queuelenVal.isUndefined()){
			queuelen = (size_t)queuelenVal.getInt64();
		}
		return std::shared_ptr<Component>{new Susi::Syscall::SyscallComponent{mgr, commands, threads, queuelen}};
	});
	registerDependency("syscallcontroller","eventsystem");
	registerDependency("syscallcontroller","iocontroller");
	registerDependency("syscallcontroller","sessionmanager");

	/**
	 * Declare httpserver
	 */
	registerComponent("httpserver", [](ComponentManager * mgr, BSON::Value& config) {
		std::string address{""};
		size_t threads{4};
		size_t backlog{16};

		if(config["address"].isString()){
			address = config["address"].getString();
		}
		std::string assetRoot{""};
		if(config["assets"].isString()){
			assetRoot = config["assets"].getString();
		}
		std::string upload{""};
		if(config["upload"].isString()){
			upload = config["upload"].getString();
		}

		if(config["threads"].isInteger()){
			threads =  config["threads"].getInt64();
		}
		if(config["backlog"].isInteger()){
			backlog =  config["backlog"].getInt64();
		}
		return std::shared_ptr<Component>{new Susi::HttpServerComponent{mgr, address, assetRoot, upload, threads, backlog}};
	});	
	registerDependency("httpserver","apiserver");
	registerDependency("httpserver","sessionmanager");
	registerDependency("httpserver","logger");
	
	/**
	 * Declare Autodiscovery
	 */
	registerComponent("autodiscovery", [](ComponentManager * mgr, BSON::Value& config){
		std::string mcast{"239.23.23.23:4242"};
		std::string ownName{"susi"};
		if(config["mcast"].isString()){
			mcast = config["mcast"].getString();
		}
		if(config["address"].isString()){
			ownName = config["address"].getString();
		}
		return std::shared_ptr<Component>{new Susi::Autodiscovery::AutoDiscoveryComponent{mcast,ownName,mgr}};
	});
	registerDependency("autodiscovery","eventsystem");
	registerDependency("autodiscovery","sessionmanager");

	/**
	 * Declare apiserver
	 */
	registerComponent("apiserver", [](ComponentManager * mgr, BSON::Value& config) {			
		return std::shared_ptr<Component>{new Susi::Api::ApiServerComponent{mgr}};
	});
	registerDependency("apiserver","eventsystem");
	registerDependency("apiserver","sessionmanager");

	/**
	 * Declare constraints
	 */
	registerComponent("constraints", [](ComponentManager * mgr, BSON::Value& config) {			
		return std::shared_ptr<Component>{new Susi::Events::ConstraintControllerComponent{mgr}};
	});
	registerDependency("constraints","eventsystem");
	registerDependency("constraints","sessionmanager");

	/**
	 * Declare DDHCP
	 */
	registerComponent("ddhcp", [](ComponentManager * mgr, BSON::Value& config) {			
		unsigned short port = 1704;
		try{
			port = (unsigned short)(int)config["port"];
		}catch(...){}
		return std::shared_ptr<Component>{new Susi::Ddhcp::DDHCPComponent{mgr,port}};
	});
	registerDependency("ddhcp","eventsystem");
	registerDependency("ddhcp","sessionmanager");

	/**
	 * Declare duktape js engine
	 */
	registerComponent("duktape",[](ComponentManager * mgr, BSON::Value& config){
		std::string source = "";
		try{
			source = config["source"].getString();
		}catch(...){}
		return std::shared_ptr<Component>{new Susi::Duktape::JSEngine{mgr,source}};
	});
	registerDependency("duktape","statecontroller");
	registerDependency("duktape","sessionmanager");

	/**
	 * Declare selfchecker
	 */
	registerComponent("selfchecker",[](ComponentManager * mgr, BSON::Value& config){
		return std::shared_ptr<Component>{new Susi::SelfCheckerComponent{mgr,config}};
	});
	registerDependency("selfchecker","eventsystem");

	/**
	 * Declare Cluster Component
	 */
	registerComponent("cluster",[](ComponentManager * mgr, BSON::Value& config){
		return std::shared_ptr<Component>{new Susi::ClusterComponent{mgr,config}};
	});
	registerDependency("cluster","eventsystem");

}
