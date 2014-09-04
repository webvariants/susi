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

#ifndef __HTTPSERVER__
#define __HTTPSERVER__

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include <iostream>

#include "apiserver/ApiServer.h"
#include "webstack/RequestHandlerFactory.h"
#include "logger/Logger.h"

namespace Susi {

class HttpServer {
protected:
	Poco::Net::SocketAddress address;
	Poco::Net::ServerSocket serverSocket;
	Poco::Net::HTTPServer server;

	Susi::Api::ApiServer apiServer;
	std::string _addr;
public:
	HttpServer(std::string addr,std::string assetRoot) :
		address(addr),
		serverSocket(address), 
		server(new RequestHandlerFactory(assetRoot, &apiServer),serverSocket,new Poco::Net::HTTPServerParams)
		{
			_addr = addr;			
		}

	HttpServer(std::string addr,std::string assetRoot,Poco::Net::HTTPServerParams *params) :
		address(addr),
		serverSocket(address),
		server(new RequestHandlerFactory(assetRoot, &apiServer),serverSocket,params)
		{
			_addr = addr;						
		}
	~HttpServer(){
		httpServerStop();
	}

	void httpServerStart() {
		server.start();
		Susi::info("started HTTP server on addr "+_addr);		
	}

	void httpServerStop() {
		server.stop();		
	}
};

}

#endif // __HTTPSERVER__