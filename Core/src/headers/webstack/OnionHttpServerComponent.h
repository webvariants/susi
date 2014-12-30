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

#ifndef __ONIONHTTPSERVERCOMPONENT__
#define __ONIONHTTPSERVERCOMPONENT__

#include "apiserver/ApiServerComponent.h"
#include "world/BaseComponent.h"
#include "sessions/SessionManager.h"
#include "apiserver/JSONStreamCollector.h"

#include <onion/exportlocal.h>
#include <onion/onion.hpp>
#include <onion/response.hpp>
#include <onion/request.hpp>
#include <onion/url.hpp>
#include <onion/types.h>
#include <onion/websocket.h>


#include <thread>

namespace Susi {
namespace Webstack {

	class HttpServerComponent : public Susi::System::BaseComponent {
	protected:

		std::shared_ptr<Susi::Sessions::SessionManager> _sessionManager;
		std::shared_ptr<Susi::Api::ApiServerComponent> _apiServer;

		Onion::Onion _server{O_POOL};
		Onion::Url _rootUrl{&_server};
		std::string _host;
		std::string _port;
		std::string _assetRoot;
		std::string _uploadDirectory;

		std::thread _runloop;

		std::string sessionHandling(Onion::Request &req, Onion::Response &res){
			auto session = req.session();
			std::string sessionID = "";
			try {
				sessionID = session["susisession"];
				std::cout<<"session found: "<<sessionID<<std::endl;	
			}catch(...){
				std::cout<<"no session found"<<std::endl;
				sessionID = std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
				session.add("susisession",sessionID);
			}
			if(!_sessionManager->checkSession(sessionID)){
				_sessionManager->updateSession(sessionID);
			}
			return sessionID;
		}

		onion_connection_status notfoundHandler(Onion::Request &req, Onion::Response &res){
            sessionHandling(req,res);
            res.setCode(404);
            res<<"not found -.-";
            return OCS_PROCESSED;
		}

		onion_connection_status redirectHandler(Onion::Request &req, Onion::Response &res){
            throw Onion::HttpRedirect{"/assets/index.html"};
		}

		onion_connection_status assetsHandler(Onion::Request &req, Onion::Response &res);

		onion_connection_status websocketHandler(Onion::Request &req, Onion::Response &res);

	public:
		HttpServerComponent(Susi::System::ComponentManager * mgr,
							std::string host,
							std::string port,
							std::string assetRoot,
							std::string uploadDirectory) :
			Susi::System::BaseComponent{mgr},
			_sessionManager{componentManager->getComponent<Susi::Sessions::SessionManager>("sessionmanager")},
			_apiServer{componentManager->getComponent<Susi::Api::ApiServerComponent>("apiserver")},
			_host{host},
			_port{port},
			_assetRoot{assetRoot},
			_uploadDirectory{uploadDirectory}
		{
			char *realPath = realpath(_assetRoot.c_str(),nullptr);
			if(realPath == nullptr){
				LOG(ERROR) << "assets base path does not exist! defaulting to './'";
				char *tmp = realpath("./",NULL);
				_assetRoot = tmp;
				_assetRoot += "/";
				free(tmp);
			}else{
				_assetRoot = realPath;
				_assetRoot += "/";
				free(realPath);
			}

			_server.setPort(_port);
			_server.setHostname(_host);
			_rootUrl.add("^assets/", this, &HttpServerComponent::assetsHandler)
					.add("^$", this, &HttpServerComponent::redirectHandler)
					.add("^ws$", this, &HttpServerComponent::websocketHandler)
					.add("^.*$", this, &HttpServerComponent::notfoundHandler);
			
		}

		virtual void start() override {
			_runloop = std::move(std::thread{[this](){
				_server.listen();
			}});
			LOG(INFO) << "started HTTP server on " << _host<<":"<<_port;
		}

		virtual void stop() override {
			_server.listenStop();
			if(_runloop.joinable()){
				_runloop.join();
			}
		}

		std::shared_ptr<Susi::Api::ApiServerComponent> apiServer(){
			return _apiServer;
		}

		~HttpServerComponent(){
			stop();
			LOG(INFO) << "stopped HTTP server";
		}

	};

}
}

#endif // __ONIONHTTPSERVERCOMPONENT__
