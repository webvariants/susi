/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 *
 * @author: Tino Rusch (tino.rusch@webvariants.de), Thomas Krause (thomas.krause@webvariants.de) 
 */

#ifndef __API_SERVER_COMPONENT__
#define __API_SERVER_COMPONENT__

#include <memory>
#include <functional>
#include <mutex>
#include "events/EventManagerComponent.h"
#include "sessions/SessionManagerComponent.h"

#include "world/BaseComponent.h"

namespace Susi {
namespace Api {

class ApiServerComponent : public Susi::System::BaseComponent {
protected:
	std::shared_ptr<Susi::Events::ManagerComponent> eventManager;
	std::shared_ptr<Susi::Sessions::SessionManagerComponent> sessionManager;

	std::map<std::string,std::function<void(Susi::Util::Any&)>> senders;
	std::map<std::string,std::map<std::string,long>> consumerSubscriptions;
	std::map<std::string,std::map<std::string,long>> processorSubscriptions;
	std::map<std::string,std::map<long,Susi::Events::EventPtr>> eventsToAck;
	
	std::mutex mutex;
	
	void handleRegisterConsumer(std::string & id, Susi::Util::Any & packet);
	void handleRegisterProcessor(std::string & id, Susi::Util::Any & packet);
	void handleUnregisterConsumer(std::string & id, Susi::Util::Any & packet);
	void handleUnregisterProcessor(std::string & id, Susi::Util::Any & packet);
	void handlePublish(std::string & id, Susi::Util::Any & packet);
	void handleAck(std::string & id, Susi::Util::Any & packet);

	void sendOk(std::string & id);
	void sendFail(std::string & id,std::string error = "");
	
	void send(std::string & id, Susi::Util::Any & msg){
		std::lock_guard<std::mutex> lock{mutex};
		auto & sender = senders[id];
		if(sender)sender(msg);
	}

public:
	
	ApiServerComponent(Susi::System::ComponentManager * mgr) : Susi::System::BaseComponent{mgr} {
		eventManager   = mgr->getComponent<Susi::Events::ManagerComponent>("eventsystem");
		sessionManager = mgr->getComponent<Susi::Sessions::SessionManagerComponent>("sessionmanager");
	}

	virtual void start() override {}
	virtual void stop() override {
		unsubscribeAll();
	}

	void onConnect(std::string & id);
	void onMessage(std::string & id, Susi::Util::Any & packet);
	void onClose(std::string & id);

	inline void registerSender(std::string id , std::function<void(Susi::Util::Any&)> sender){
		std::lock_guard<std::mutex> lock{mutex};
		senders[id] = sender;
	}
	inline void unregisterSender(std::string id){
		std::lock_guard<std::mutex> lock{mutex};
		senders.erase(id);
	}

};

}
}

#endif // __API_SERVER_COMPONENT__