#include "world/World.h"
#include "events/global.h"
 
Susi::World world;

void Susi::World::setup(){
	setupLogger();
	setupEventSystem();
	setupEventManager();
	//setupTCPServer();
	setupHttpServer();
	setupHeartBeat();
	setupSessionManager();
	setupDBManager();
	setupIOController();
	setupEngineStarter();
	setupAuthController();
	setupStateController();
	setupJSEngine();
	//setupSysCallController();
}

void Susi::World::tearDown(){

}

void Susi::World::setupJSEngine(){
	std::string file = "./controller.js";
	try{
		auto & app = Poco::Util::Application::instance();
		auto & cfg = app.config();
		file = cfg.getString("jsengine.source");
	}catch(const std::exception & e){}
	jsEngine = std::shared_ptr<Susi::JSEngine>(new Susi::JSEngine(file));
}

void Susi::World::setupEventManager(){
	eventManager = std::shared_ptr<Susi::Events::Manager>(new Susi::Events::Manager{});
}

void Susi::World::setupStateController(){
	std::string file = "./states.json";
	try{
		auto & app = Poco::Util::Application::instance();
		auto & cfg = app.config();
		file = cfg.getString("states.file");
	}catch(const std::exception & e){}
	stateController = std::shared_ptr<Susi::States::StateController>{
		new Susi::States::StateController(file)
	};

	Susi::States::EventInterface::init();
}

void Susi::World::setupEngineStarter(){
	std::string base = "./controller/";
	try{
		auto & app = Poco::Util::Application::instance();
		auto & cfg = app.config();
		base = cfg.getString("enginestarter.root");
	}catch(const std::exception & e){}
	engineStarter = std::shared_ptr<Susi::EngineStarter>{new Susi::EngineStarter(base)};
}

void Susi::World::setupEventSystem(){
	eventSystem = std::shared_ptr<Susi::EventSystem>{new Susi::EventSystem()};
}


void Susi::World::setupLogger(){
	unsigned char level = Susi::Logger::NOTHING;
	try{
		auto & app = Poco::Util::Application::instance();
		auto & cfg = app.config();
		level = cfg.getInt("logger.level");
	}catch(const std::exception & e){}
	logger = std::shared_ptr<Susi::Logger>{new Susi::Logger( level )};
}

/*void Susi::World::setupTCPServer(){
	std::string addr = "[::1]:4000";
	try{
		auto & app = Poco::Util::Application::instance();
		auto & cfg = app.config();
		addr = cfg.getString("tcpserver.addr");
	}catch(const std::exception & e){}
	auto tcpParams = new Poco::Net::TCPServerParams();
	tcpParams->setMaxQueued(250);
	tcpServer = std::shared_ptr<Susi::TCPServer>{new Susi::TCPServer(addr,tcpParams)};
}*/

void Susi::World::setupHttpServer(){
	std::string addr = "[::1]:8080";
	std::string assets = "./assets/";
	try{
		auto & app = Poco::Util::Application::instance();
		auto & cfg = app.config();
		addr = cfg.getString("webstack.addr");
		assets = cfg.getString("webstack.assets");
	}catch(const std::exception & e){}
	auto httpParams = new Poco::Net::HTTPServerParams();
	httpParams->setMaxQueued(250);
	httpServer = std::shared_ptr<Susi::HttpServer>{new Susi::HttpServer(addr,assets,httpParams)};
}

void Susi::World::setupSessionManager(){
	int lifetime = 20;
	int interval = 5;
	try{
		auto & app = Poco::Util::Application::instance();
		auto & cfg = app.config();
		lifetime = cfg.getInt("session.lifetime");
		interval = cfg.getInt("session.interval");
	}catch(const std::exception & e){}
	sessionManager = std::shared_ptr<Susi::Sessions::SessionManager>{new Susi::Sessions::SessionManager()};
	sessionManager->init(std::chrono::milliseconds(lifetime*1000),std::chrono::milliseconds(interval*1000));
	Susi::Sessions::initEventInterface();
}

void Susi::World::setupIOController() {
	ioController = std::shared_ptr<Susi::IOController>{new Susi::IOController()};
	Susi::IOEventInterface::initEventInterface();
}

void Susi::World::setupHeartBeat() {
	heartBeat = std::shared_ptr<Susi::HeartBeat>{new Susi::HeartBeat()};
}

void Susi::World::setupDBManager(){
	std::vector<std::tuple<std::string,std::string,std::string>> dbs;
	std::string dbConfigPath = "./dbs.conf";
	try{
		auto & app = Poco::Util::Application::instance();
		auto & cfg = app.config();
		dbConfigPath = cfg.getString("db.config");
	}catch(const std::exception & e){}
	
	try{
		IOController io;		
		std::string configData = io.readFile(dbConfigPath);
				
		if(configData != ""){
			Poco::JSON::Parser parser;
			Poco::Dynamic::Var var = parser.parse(configData);
			Poco::JSON::Array::Ptr array = var.extract<Poco::JSON::Array::Ptr>();
			for(std::size_t i=0; i<array->size(); i++){
				Poco::JSON::Object::Ptr obj = array->getObject(i);
				auto tuple = std::make_tuple(obj->get("name").toString(),
											 obj->get("type").toString(),
											 obj->get("uri").toString());
				dbs.push_back(tuple);
			}
		} 			
	}catch(const std::exception & e){
		std::string msg = "Execption while setup DBManager: ";
		msg += e.what();
		Susi::warn(msg);
		dbs.push_back(std::make_tuple("auth","sqlite3","./auth.db"));
	}
	dbManager = std::shared_ptr<Susi::DB::Manager>{new Susi::DB::Manager(dbs)};
	Susi::DB::init();
}

void Susi::World::setupAuthController() {
	std::string dbIdentifier = "auth";
	try{
		auto & app = Poco::Util::Application::instance();
		auto & cfg = app.config();
		dbIdentifier = cfg.getString("authentification.db");
	}catch(const std::exception & e){}
	authController = std::shared_ptr<Susi::Auth::Controller>{new Susi::Auth::Controller(dbIdentifier)};

	Susi::Auth::EventInterface::init();
}

void Susi::World::setupSysCallController() {
	std::string configPath = "./test_config/unified.cfg";
	try{
		auto & app = Poco::Util::Application::instance();
		auto & cfg = app.config();
		configPath = cfg.getString("syscall.cfg");
	}catch(const std::exception & e){}
	syscallController = std::shared_ptr<Susi::Syscall::Controller>{new Susi::Syscall::Controller(configPath)};

	Susi::Syscall::EventInterface::init();
}