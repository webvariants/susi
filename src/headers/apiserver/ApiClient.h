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

#ifndef __APICLIENT__
#define __APICLIENT__

#include "apiserver/JSONTCPClient.h"
#include "events/Manager.h"

namespace Susi {
namespace Api {

class ApiClient : public JSONTCPClient, public Susi::Events::Manager {
public:
	ApiClient(std::string host, unsigned short port) : JSONTCPClient{host,port} {}
	void publish(Susi::Events::EventPtr event, Susi::Events::Consumer finishHandler);
	void subscribe(std::string topic, Susi::Events::Processor processor);
	void subscribe(std::string topic, Susi::Events::Consumer consumer);
protected:
	virtual void onMessage(Susi::Util::Any & message) override;

	void handleEvent(Susi::Events::EventPtr event);
	void handleAck(Susi::Events::EventPtr event);
	void handleStatus(bool error,std::string message);
};

}	
}


#endif // __APICLIENT__