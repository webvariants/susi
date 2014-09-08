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

#ifndef __TCPAPISERVERCOMPONENT__
#define __TCPAPISERVERCOMPONENT__

#include "Poco/Net/TCPServer.h"
#include "Poco/Net/TCPServerConnectionFactory.h"
#include "Poco/Net/TCPServerConnection.h"
#include "Poco/Net/Socket.h"
#include "logger/Logger.h"
#include "apiserver/ApiServerForComponent.h"
#include "apiserver/JSONStreamCollector.h"
#include "world/BaseComponent.h"

namespace Susi {
namespace Api {



class TCPApiServerComponent : public Susi::System::BaseComponent {
protected:
	class Connection : public Poco::Net::TCPServerConnection {
	protected:
		Susi::Api::ApiServerForComponent api;
		std::string sessionID = "";
		Susi::Api::JSONStreamCollector collector;
	public:
		Connection(const Poco::Net::StreamSocket& s, std::shared_ptr<Susi::Events::ManagerComponent> eventManager) :
			Poco::Net::TCPServerConnection{s},
			api{eventManager},
			sessionID{std::to_string(std::chrono::system_clock::now().time_since_epoch().count())},
			collector{[this](std::string & msg){
				std::cout<<"got message in server! "<<msg<<std::endl;
				std::string s = sessionID;
				auto message = Susi::Util::Any::fromString(msg);
				api.onMessage(s,message);
			}} {
				api.onConnect(sessionID);
				api.registerSender(sessionID,[this](Susi::Util::Any & msg) {
					if(this==nullptr) return;
					std::string str = msg.toString()+"\n";
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
		std::shared_ptr<Susi::Events::ManagerComponent> eventManager;
		ConnectionFactory(std::shared_ptr<Susi::Events::ManagerComponent> _eventManager) {
			eventManager = _eventManager;
		}
		virtual Poco::Net::TCPServerConnection * createConnection(const Poco::Net::StreamSocket& s){
			//std::cout<<"create connection!"<<std::endl;
			return new Connection{s, eventManager};
		}
	};

	Poco::Net::SocketAddress address;
	Poco::Net::ServerSocket serverSocket;
	Poco::Net::TCPServerParams *params;
	Poco::Net::TCPServer tcpServer;

public:
	TCPApiServerComponent(Susi::System::ComponentManager * mgr, std::string addr, size_t threads = 4, size_t backlog = 16) :
		Susi::System::BaseComponent{mgr},
		address{addr},
		serverSocket{address},
		params{new Poco::Net::TCPServerParams},
		tcpServer{new ConnectionFactory{eventManager},serverSocket,params} {
			params->setMaxThreads(threads);
			params->setMaxQueued(backlog);
			params->setThreadIdleTime(100);
	}
	virtual void start() override {
		tcpServer.start();
		std::string msg{"started TCP Api Server on "};
		msg += address.toString();
		Susi::Logger::info(msg);
		std::this_thread::sleep_for(std::chrono::milliseconds{250});
	}
	virtual void stop() override {
		tcpServer.stop();
	}
	~TCPApiServerComponent(){
		stop();
	}

};

}
}

#endif // __TCPAPISERVERCOMPONENT__
