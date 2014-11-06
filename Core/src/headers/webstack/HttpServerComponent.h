/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 *
 * @author: Thomas Krause (thomas.krause@webvariants.de)
 */

#ifndef __HTTP_SERVER_COMPONENT__
#define __HTTP_SERVER_COMPONENT__

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include <iostream>

#include "apiserver/ApiServerComponent.h"
#include "webstack/RequestHandlerFactory.h"
#include "logger/easylogging++.h"
#include "world/BaseComponent.h"

namespace Susi {

class HttpServerComponent : public Susi::System::BaseComponent {
protected:
	std::shared_ptr<Susi::Api::ApiServerComponent> _api;
	Poco::Net::SocketAddress address;
	Poco::Net::ServerSocket serverSocket;
	Poco::Net::HTTPServerParams *params;
	Poco::Net::HTTPServer server;

	std::string _addr;
public:
	HttpServerComponent (Susi::System::ComponentManager * mgr,
						 std::string addr,
						 std::string assetRoot,
						 std::string uploadDirectory,
						 size_t threads = 4,
						 size_t backlog = 16) :
		Susi::System::BaseComponent{mgr},
		_api{mgr->getComponent<Susi::Api::ApiServerComponent>("apiserver")},
		address{addr},
		serverSocket{address},
		params{new Poco::Net::HTTPServerParams},
		server(new RequestHandlerFactory(assetRoot, uploadDirectory, _api, mgr->getComponent<Susi::Sessions::SessionManagerComponent>("sessionmanager")),serverSocket,params)
		{
			_addr = addr;
			params->setMaxThreads(threads);
			params->setMaxQueued(backlog);
		}

	virtual void start() override {
		server.start();
		LOG(INFO) << "started HTTP server on addr "+_addr;
	}

    virtual void stop() override {
        server.stop();
    }

    ~HttpServerComponent() {
        stop();
        LOG(INFO) <<  "stopped HTTPServerComponent" ;
    }
};

}

#endif // __HTTP_SERVER_COMPONENT__
