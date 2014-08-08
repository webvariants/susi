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

#ifndef __APISERVER__
#define __APISERVER__

#include <memory>
#include "apiserver/JSONTCPServer.h"
#include "events/global.h"

namespace Susi {
namespace Api {

class ApiServer : JSONTCPServer {
public:
	ApiServer(unsigned short port) : JSONTCPServer{port} {}
protected:
	
	std::map<std::string,std::map<std::string,long>> subscriptions;
	std::map<std::string,std::map<long,Susi::Events::EventPtr>> eventsToAck;

	virtual void onMessage(std::string & id, Susi::Util::Any & packet) override;
	
	void handleRegisterConsumer(std::string & id, Susi::Util::Any & packet);
	void handleRegisterProcessor(std::string & id, Susi::Util::Any & packet);
	void handleUnregister(std::string & id, Susi::Util::Any & packet);
	void handlePublish(std::string & id, Susi::Util::Any & packet);
	void handleAck(std::string & id, Susi::Util::Any & packet);

	void handleMalformedPacket(std::string & id, Susi::Util::Any & packet);
	
	void sendOk(std::string & id);
	void sendFail(std::string & id);

};

}
}

#endif // __APISERVER__
