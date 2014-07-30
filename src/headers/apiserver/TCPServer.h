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

#ifndef __TCPSERVER__
#define __TCPSERVER__

#include <Poco/Net/TCPServer.h>
#include <Poco/Net/TCPServerParams.h>
#include <Poco/Net/TCPServerConnectionFactory.h>
#include <Poco/Net/TCPServerConnection.h>
#include <Poco/Net/Socket.h>

#include <memory>

#include "apiserver/Connection.h" 
#include "apiserver/ConnectionHandler.h" 
#include "logger/Logger.h"

namespace Susi {

class TCPConnectionWrapper : public Poco::Net::TCPServerConnection {
public:
	TCPConnectionWrapper(const Poco::Net::StreamSocket& s) : Poco::Net::TCPServerConnection(s) {}
	void run(){
		try{
			std::shared_ptr<Connection> connection(new TCPConnection(socket()));
			Susi::ConnectionHandler::handleConnection(connection);
		}catch(const std::exception & e){
			std::string msg = "Error while handling tcp connection: ";
			msg += e.what();
			Susi::error(msg);
		}
	}
};

class TCPServer{
protected:
	Poco::Net::SocketAddress address;
	Poco::Net::ServerSocket serverSocket;
	Poco::Net::TCPServer server;
public:
	TCPServer(std::string addr,Poco::Net::TCPServerParams *params) : 
		address(addr),
		serverSocket(address,64), 
		server(new Poco::Net::TCPServerConnectionFactoryImpl<TCPConnectionWrapper>(),serverSocket,params)
		{
			server.start();
			Susi::info("started TCP server on "+addr);
		}	
	~TCPServer(){
		server.stop();
	}
};

}

#endif // __TCPSERVER__