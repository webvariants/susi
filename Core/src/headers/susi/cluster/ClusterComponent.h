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

#ifndef __CLUSTERCOMPONENT__
#define __CLUSTERCOMPONENT__

#include "susi/world/BaseComponent.h"
#include "susi/logger/easylogging++.h"

#include "susi/apiserver/ApiClient.h"

namespace Susi {

class ClusterComponent : public Susi::System::BaseComponent {
protected:
	BSON::Array nodes;
	//std::vector<std::shared_ptr<Susi::Api::ApiClient>> apiClients;
	std::map<std::string,std::shared_ptr<Susi::Api::ApiClient>> apiClients;
	std::deque<std::string> eventBlacklist;

	void addToBlacklist(std::string & id);
	bool checkIfInBlacklist(std::string & id);

	void validateNode(BSON::Value & node);
	void setupNode(BSON::Value & node);
	void registerProcessorForNode(std::string nodeId, std::string topic);
	void registerConsumerForNode(std::string nodeId, std::string topic);
	void registerForwardingForNode(std::string nodeId, std::string topic);

public:
	ClusterComponent(Susi::System::ComponentManager *mgr, BSON::Value config) : Susi::System::BaseComponent{mgr} {
		if(!config.isObject() || !config["nodes"].isArray()){
			throw std::runtime_error{"ClusterComponent: config is malformed."};
		}
		nodes = config["nodes"].getArray();
	}

	virtual void start() override {
		for(auto & node : nodes){
			validateNode(node);
			setupNode(node);
		}
		subscribe(std::string{"cluster::registerProcessor"},[this](Susi::Events::EventPtr evt){
			if(!evt->payload.isObject() || !evt->payload["node"].isString() || !evt->payload["topic"].isString()){
				throw std::runtime_error{"you must specify 'node' and 'topic'"};
			}
			registerProcessorForNode(evt->payload["node"].getString(), evt->payload["topic"].getString());
		});
		subscribe(std::string{"cluster::registerConsumer"},[this](Susi::Events::EventPtr evt){
			if(!evt->payload.isObject() || !evt->payload["node"].isString() || !evt->payload["topic"].isString()){
				throw std::runtime_error{"you must specify 'node' and 'topic'"};
			}
			registerConsumerForNode(evt->payload["node"].getString(), evt->payload["topic"].getString());
		});
		subscribe(std::string{"cluster::forward"},[this](Susi::Events::EventPtr evt){
			if(!evt->payload.isObject() || !evt->payload["node"].isString() || !evt->payload["topic"].isString()){
				throw std::runtime_error{"you must specify 'node' and 'topic'"};
			}
			registerForwardingForNode(evt->payload["node"].getString(), evt->payload["topic"].getString());
		});
	}

	virtual void stop() override {
		for(auto & kv : apiClients){
			kv.second->close();
		}
		apiClients.clear();
	}

	~ClusterComponent(){
		stop();
	}

};
	
}

#endif // __CLUSTERCOMPONENT__
