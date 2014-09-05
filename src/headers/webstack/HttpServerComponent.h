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

#include "apiserver/ApiServerForComponent.h"
#include "webstack/RequestHandlerFactory.h"
#include "logger/Logger.h"
#include "world/BaseComponent.h"

namespace Susi {

class HttpServerComponent : public Susi::System::BaseComponent {
protected:
	Susi::Api::ApiServerForComponent apiServer;
	Poco::Net::SocketAddress address;
	Poco::Net::ServerSocket serverSocket;
	Poco::Net::HTTPServer server;

	std::string _addr;
public:
	HttpServerComponent (Susi::System::ComponentManager * mgr, std::string addr,std::string assetRoot) :
		Susi::System::BaseComponent{mgr},
		apiServer{eventManager},
		address(addr),
		serverSocket(address),
		server(new RequestHandlerFactory(assetRoot, &apiServer),serverSocket,new Poco::Net::HTTPServerParams)
		{
			_addr = addr;
	}

	virtual void start() override {
		server.start();
		Susi::info("started HTTP server on addr "+_addr);
	}

	virtual void stop() override {
		server.stop();
	}
	~HttpServerComponent() {
		stop();
	}
};

}

#endif // __HTTP_SERVER_COMPONENT__
