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

#include "susi/world/ComponentManager.h"
#include "susi/logger/easylogging++.h"

#include "susi/apiserver/ApiClient.h"

namespace Susi {

class ClusterComponent : public Susi::System::BaseComponent {
protected:
	BSON::Array nodes;
	std::vector<std::shared_ptr<Susi::Api::ApiClient>> apiClients;
	std::deque<std::string> eventBlacklist;

	void addToBlacklist(std::string & id){
		eventBlacklist.push_front(id);
		while(eventBlacklist.size() > 64){
			eventBlacklist.pop_back();
		}
	}

	bool checkIfInBlacklist(std::string & id){
		for(auto & entry : eventBlacklist){
			if(entry == id){
				return true;
			}
		}
		return false;
	}

public:
	ClusterComponent(Susi::System::ComponentManager *mgr, BSON::Value config) : Susi::System::BaseComponent{mgr} {
		if(!config.isObject() || !config["nodes"].isArray()){
			throw std::runtime_error{"ClusterComponent: config is malformed."};
		}
		nodes = config["nodes"].getArray();
	}

	virtual void start() override {
		for(auto & node : nodes){
			if(!node.isObject() || !node["addr"].isString()){
				throw std::runtime_error{"ClusterComponent: config is malformed."};
			}
			auto apiClient = std::make_shared<Susi::Api::ApiClient>(node["addr"].getString());
			apiClients.push_back(apiClient);
			if(node["topics"].isArray()){
				for(auto & topic : node["topics"].getArray()){
					if(!topic.isString()){
						throw std::runtime_error{"ClusterComponent: config is malformed."};
					}

					Susi::Events::Processor processor = [this,apiClient](Susi::Events::EventPtr event){
						LOG(DEBUG) << "received event from other cluster node: "<<event->toString();
						struct FinishCallback {
							Susi::Events::EventPtr mainEvent;
							std::shared_ptr<Susi::Api::ApiClient> apiClient;
							FinishCallback(Susi::Events::EventPtr evt) : 
								mainEvent{std::move(evt)} {}
							FinishCallback(FinishCallback && other) : 
								mainEvent{std::move(other.mainEvent)} {}
							FinishCallback(FinishCallback & other) : 
								mainEvent{std::move(other.mainEvent)} {}
							void operator()(Susi::Events::SharedEventPtr subEvent){
								LOG(DEBUG) << "in finish callback";
								*mainEvent = *subEvent;
							}
						};
						if(!checkIfInBlacklist(event->id)){
							LOG(DEBUG) << "event not in blacklist";
							addToBlacklist(event->id);
							auto evt = createEvent(event->topic);
							*evt = *event;
							publish(std::move(evt),FinishCallback{std::move(event)});
						}else{
							LOG(DEBUG) << "event is blacklisted!";
						}
					};

					apiClient->subscribe(topic.getString(),processor);
					LOG(INFO) << "subscribed to topic "<<topic.getString()<<" @ "<<node["addr"].getString();
				}
			}
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
