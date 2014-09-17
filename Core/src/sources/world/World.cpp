#include "world/World.h"
#include "events/global.h"
 
Susi::World world;

void Susi::World::setConfig(Susi::Config *_cfg) {
	cfg = _cfg;
}

void Susi::World::setup(){
	std::cout<<"WORLD setup:"<<std::endl;
	setupEventManager();
	setupTCPServer();
	setupSessionManager();
	setupIOController();
	setupStateController();
}

void Susi::World::tearDown(){
	tcpServer.reset();
	httpServer.reset();
	authController.reset();
	ioController.reset();
	stateController.reset();
	sessionManager.reset();
	eventManager.reset();
	logger.reset();
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

void Susi::World::setupTCPServer(){
	std::string addr = "[::1]:4000";
	try{
		auto & app = Poco::Util::Application::instance();
		auto & cfg = app.config();
		addr = cfg.getString("tcpserver.addr");
	}catch(const std::exception & e){}
	tcpServer = std::make_shared<Susi::Api::TCPApiServer>(addr);
}

void Susi::World::setupSessionManager(){
	int lifetime = 20;	
	try{
		auto & app = Poco::Util::Application::instance();
		auto & cfg = app.config();
		lifetime = cfg.getInt("session.lifetime");
	}catch(const std::exception & e){}
	sessionManager = std::shared_ptr<Susi::Sessions::SessionManager>{new Susi::Sessions::SessionManager()};
	sessionManager->init(std::chrono::milliseconds(lifetime*1000));
	Susi::Sessions::initEventInterface();
}

void Susi::World::setupIOController() {
	ioController = std::shared_ptr<Susi::IOController>{new Susi::IOController()};
	Susi::IOEventInterface::initEventInterface();
}
