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

#ifndef __JSONTCPAPISERVER__
#define __JSONTCPAPISERVER__

#include "apiserver/JSONTCPServer.h"
#include "apiserver/ApiServer.h"

namespace Susi{
namespace Api{
	class JsonTcpApiServer : public JSONTCPServer {
	protected:
		ApiServer *apiServer;
		virtual void onConnect(std::string & id) override {
			JSONTCPServer::onConnect(id);
			apiServer->registerSender(id,[this,id](Susi::Util::Any & msg){
				std::string _id{id};
				this->send(_id,msg);
			});
			apiServer->onConnect(id);
		}
		virtual void onMessage(std::string & id, Susi::Util::Any & message) override {
			apiServer->onMessage(id,message);
		}
		virtual void onClose(std::string & id) override {
			JSONTCPServer::onClose(id);
			apiServer->onClose(id);
		}
	public:
		JsonTcpApiServer(unsigned short port,ApiServer* server) : JSONTCPServer{port}, apiServer{server} {}
	};
}
}

#endif // __JSONTCPAPISERVER__