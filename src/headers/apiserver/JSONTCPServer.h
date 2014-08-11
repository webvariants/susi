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

#ifndef __JSONTCPSERVER__
#define __JSONTCPSERVER__

#include "apiserver/TCPServer.h"
#include "apiserver/JSONStreamCollector.h"
#include "util/Any.h"

namespace Susi {
namespace Api {

class JSONTCPServer : public TCPServer {
public:
	JSONTCPServer(unsigned short port) : TCPServer{port} {}
protected:
	virtual void onMessage(std::string & id, Susi::Util::Any & message) = 0;
	std::map<std::string,JSONStreamCollector> collectors;
	virtual void onConnect(std::string & id) override {
		collectors[id] = JSONStreamCollector{[this,id](std::string & msg){
			auto message = Susi::Util::Any::fromString(msg);
			this->onMessage((std::string&)id,message);
		}};
	}
	virtual void onData(std::string & id, std::string & data) override {
		collectors[id].collect(data);
	}
	virtual void onClose(std::string & id) override {
		collectors.erase(id);
	}

	void send(std::string & id, Susi::Util::Any & message){
		std::string msg = message.toString()+"\n";
		TCPServer::send(id,msg);
	}
};

}	
}


#endif // __JSONTCPSERVER__