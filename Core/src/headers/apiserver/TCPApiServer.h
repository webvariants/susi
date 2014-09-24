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
#include "apiserver/JSONStreamCollector.h"
#include "logger/Logger.h"

namespace Susi {
namespace Api {



class TCPApiServer {
protected:
	class Connection : public Poco::Net::TCPServerConnection {
	protected:
		ApiServer api;
		std::string sessionID = "";
		JSONStreamCollector collector;
	public:
		Connection(const Poco::Net::StreamSocket& s) :
				Poco::Net::TCPServerConnection{s},
				sessionID{std::to_string(std::chrono::system_clock::now().time_since_epoch().count())},
				collector{[this](std::string & msg){
					std::cout<<"got message in server! "<<msg<<std::endl;
					std::string s = sessionID;
					auto message = Susi::Util::Any::fromJSONString(msg);
					api.onMessage(s,message);
				}} {
			api.onConnect(sessionID);
			api.registerSender(sessionID,[this](Susi::Util::Any & msg){
				if(this==nullptr)return;
				std::string str = msg.toJSONString()+"\n";
				socket().sendBytes(str.c_str(),str.size());
			});
		}
		~Connection(){
			api.onClose(sessionID);
		}
		void run(){
			char buff[1024];
			while(true){
				int bs = socket().receiveBytes(buff,sizeof(buff));
				if(bs<=0){
					socket().shutdown();
					break;
				}
				std::string s{buff,static_cast<size_t>(bs)};
				//std::cout<<"got data in server: "<<s<<std::endl;
				collector.collect(s);
			}
		}
	};

	class ConnectionFactory : public Poco::Net::TCPServerConnectionFactory {
	public:
		virtual Poco::Net::TCPServerConnection * createConnection(const Poco::Net::StreamSocket& s){
			//std::cout<<"create connection!"<<std::endl;
			return new Connection{s};
		}
	};

	Poco::Net::SocketAddress address;
	Poco::Net::ServerSocket serverSocket;
	Poco::Net::TCPServerParams *params;
	Poco::Net::TCPServer tcpServer;

public:
	TCPApiServer(std::string addr, size_t threads = 4, size_t backlog = 16) :
		address{addr},
		serverSocket{address},
		params{new Poco::Net::TCPServerParams},
		tcpServer{new ConnectionFactory{},serverSocket,params} {
			params->setMaxThreads(threads);
			params->setMaxQueued(backlog);
			params->setThreadIdleTime(100);
			tcpServer.start();
			std::string msg{"started TCP Api Server on "};
			msg += addr;
			Susi::Logger::info(msg);
			std::this_thread::sleep_for(std::chrono::milliseconds{250});
	}
	~TCPApiServer(){
		tcpServer.stop();
	}

};

}
}

#endif // __TCPAPISERVER__
