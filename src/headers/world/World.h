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

#ifndef __WORLD__
#define __WORLD__

#include <memory>
#include <Poco/Util/Application.h>
#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Array.h>

#include "events/EventSystem.h"
#include "apiserver/TCPServer.h"
#include "webstack/HttpServer.h"
#include "sessions/SessionManager.h"
#include "sessions/SessionManagerEventInterface.h"
#include "tiny-js/JSEngine.h"
#include "db/Manager.h"
#include "db/EventInterface.h"
#include "iocontroller/IOController.h"
#include "heartbeat/HeartBeat.h"
#include "iocontroller/IOEventInterface.h"
#include "logger/Logger.h"
#include "enginestarter/EngineStarter.h"
#include "auth/AuthController.h"
#include "auth/AuthEventInterface.h"
#include "states/StateController.h"
#include "events/Manager.h"
#include "syscall/SysCallEventInterface.h"
#include "syscall/SysCallController.h"

namespace Susi{

	class World {
	protected:		
	public:
		void setupEventSystem();
		void setupEventManager();
		void setupTCPServer();
		void setupHttpServer();
		void setupSessionManager();
		void setupJSEngine();
		void setupIOController();
		void setupHeartBeat();
		void setupDBManager();
		void setupLogger();
		void setupEngineStarter();
		void setupAuthController();
		void setupSysCallController();
		void setupStateController();

		std::shared_ptr<Susi::EventSystem> 				eventSystem{nullptr};
		std::shared_ptr<Susi::Events::Manager> 			eventManager{nullptr};
		std::shared_ptr<Susi::TCPServer> 				tcpServer{nullptr};
		std::shared_ptr<Susi::HttpServer> 				httpServer{nullptr};
		std::shared_ptr<Susi::Sessions::SessionManager> sessionManager{nullptr};
		std::shared_ptr<Susi::JSEngine> 				jsEngine{nullptr};
		std::shared_ptr<Susi::IOController>				ioController{nullptr};
		std::shared_ptr<Susi::HeartBeat>				heartBeat{nullptr};
		std::shared_ptr<Susi::DB::Manager>				dbManager{nullptr};
		std::shared_ptr<Susi::Logger>					logger{nullptr};
		std::shared_ptr<Susi::EngineStarter>			engineStarter{nullptr};
		std::shared_ptr<Susi::Auth::Controller>			authController{nullptr};
		std::shared_ptr<Susi::States::StateController>	stateController{nullptr};
		std::shared_ptr<Susi::Syscall::Controller>		syscallController{nullptr};

		void setup();
		void tearDown();
		~World(){
			tearDown();
		}
	};
}

extern Susi::World world;

#endif // __WORLD__