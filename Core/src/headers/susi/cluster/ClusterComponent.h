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
	}

	virtual void stop() override {
		apiClients.clear();
	}

	~ClusterComponent(){
		stop();
	}

};
	
}

#endif // __CLUSTERCOMPONENT__
