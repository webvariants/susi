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

#ifndef __TCPAPISERVER__
#define __TCPAPISERVER__

#include "Poco/Net/TCPServer.h"
#include "Poco/Net/TCPServerConnectionFactory.h"
#include "Poco/Net/TCPServerConnection.h"
#include "Poco/Net/Socket.h" 
#include "apiserver/ApiServer.h"
#include "apiserver/JSONStreamCollector.h"

namespace Susi {
namespace Api {



class TCPApiServer {
protected:
	class Connection : public Poco::Net::TCPServerConnection {
	protected:
		ApiServer * api;
	public:
		Connection(const Poco::Net::StreamSocket& s, ApiServer * apiserver) : Poco::Net::TCPServerConnection{s}, api{apiserver} {}
		void run(){
			std::string sessionID = std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
			auto sock = socket();
			api->registerSender(sessionID,[&sock](Susi::Util::Any & msg){
				std::string str = msg.toString()+"\n";
				sock.sendBytes(str.c_str(),str.size());
			});
			api->onConnect(sessionID);
			//std::cout<<"got client!"<<std::endl;
			JSONStreamCollector collector{[sessionID,this](std::string & msg){
				std::string s = sessionID;
				auto message = Susi::Util::Any::fromString(msg);
				api->onMessage(s,message);
				//std::cout<<"got message!"<<std::endl;
			}};
			char buff[1024];
			while(true){
				size_t bs = socket().receiveBytes(buff,sizeof(buff));
				if(bs<=0){
					socket().close();
					api->onClose(sessionID);
					//std::cout<<"lost client..."<<std::endl;
					break;
				}
				std::string s{buff,bs};
				//std::cout<<"got data!"<<std::endl;
				collector.collect(s);
			}
		}
	};

	class ConnectionFactory : public Poco::Net::TCPServerConnectionFactory {
	protected:
		ApiServer * apiserver;
	public:
		ConnectionFactory(ApiServer * api) : apiserver{api} {}
		virtual Poco::Net::TCPServerConnection * createConnection(const Poco::Net::StreamSocket& s){
			return new Connection{s,apiserver};
		}
	};

	Poco::Net::SocketAddress address;
	Poco::Net::ServerSocket serverSocket;
	Poco::Net::TCPServerParams *params;
	Poco::Net::TCPServer tcpServer;
	ApiServer api;

public:
	TCPApiServer(std::string addr, size_t threads = 4, size_t backlog = 16) : 
		address{addr},
		serverSocket{address}, 
		params{new Poco::Net::TCPServerParams}, 
		tcpServer{new ConnectionFactory{&api},serverSocket,params} {
			params->setMaxThreads(threads);
			params->setMaxQueued(backlog);
			params->setThreadIdleTime(100);
			tcpServer.start();
	}

};

}
}

#endif // __TCPAPISERVER__
