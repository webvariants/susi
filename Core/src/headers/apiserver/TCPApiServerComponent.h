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
#include "apiserver/ApiServerComponent.h"
#include "apiserver/JSONStreamCollector.h"
#include "world/BaseComponent.h"

namespace Susi {
namespace Api {



class TCPApiServerComponent : public Susi::System::BaseComponent {
protected:
	class Connection : public Poco::Net::TCPServerConnection {
	protected:
		std::shared_ptr<Susi::Api::ApiServerComponent> _api;
		std::string sessionID = "";
		Susi::Api::JSONStreamCollector collector;
	public:
		Connection(const Poco::Net::StreamSocket& s,std::shared_ptr<Susi::Api::ApiServerComponent> api) :
			Poco::Net::TCPServerConnection{s},
			_api{api},
			sessionID{std::to_string(std::chrono::system_clock::now().time_since_epoch().count())},
			collector{[this](std::string & msg){
				Susi::Logger::debug("got message in collector!");
				std::string s = sessionID;
				auto message = Susi::Util::Any::fromJSONString(msg);
				_api->onMessage(s,message);
			}} {
				Susi::Logger::debug("Connection constructor");
				_api->onConnect(sessionID);
				_api->registerSender(sessionID,[this](Susi::Util::Any & msg) {
					if(this==nullptr) return;
					std::string str = msg.toJSONString()+"\n";
					socket().sendBytes(str.c_str(),str.size());
			});
		}
		~Connection(){
			_api->onClose(sessionID);
			Susi::Logger::debug("deleting tcp connection");
		}
		void run(){
			char buff[1024];
			while(true){
				int bs = socket().receiveBytes(buff,sizeof(buff));
				if(bs<=0){
					Susi::Logger::debug("tcp connection failed while receive.");
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
		std::shared_ptr<Susi::Api::ApiServerComponent> _api;

		ConnectionFactory(std::shared_ptr<Susi::Api::ApiServerComponent> api) : _api{api} {}
		virtual Poco::Net::TCPServerConnection * createConnection(const Poco::Net::StreamSocket& s){
			//std::cout<<"create connection!"<<std::endl;
			Susi::Logger::debug("creating new tcp connection");
			return new Connection{s, _api};
		}
	};

	std::shared_ptr<Susi::Api::ApiServerComponent> api;
	Poco::Net::SocketAddress address;
	Poco::Net::ServerSocket serverSocket;
	Poco::Net::TCPServerParams *params;
	Poco::Net::TCPServer tcpServer;
public:
	TCPApiServerComponent(Susi::System::ComponentManager * mgr,
						  std::string addr,
						  size_t threads = 4,
						  size_t backlog = 16) :
		Susi::System::BaseComponent{mgr},
		api{componentManager->getComponent<Susi::Api::ApiServerComponent>("apiserver")},
		address{addr},
		serverSocket{address},
		params{new Poco::Net::TCPServerParams},
		tcpServer{new ConnectionFactory{api}, serverSocket,	params} {
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
		Susi::Logger::info("stopped TCP Api Server");
	}
	~TCPApiServerComponent(){
		stop();
	}

};

}
}

#endif // __TCPAPISERVERCOMPONENT__
