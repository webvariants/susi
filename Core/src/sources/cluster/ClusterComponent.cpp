#include "susi/cluster/ClusterComponent.h"

void Susi::ClusterComponent::addToBlacklist(std::string & id){
	eventBlacklist.push_front(id);
	while(eventBlacklist.size() > 64){
		eventBlacklist.pop_back();
	}
}

bool Susi::ClusterComponent::checkIfInBlacklist(std::string & id){
	for(auto & entry : eventBlacklist){
		if(entry == id){
			return true;
		}
	}
	return false;
}

void Susi::ClusterComponent::validateNode(BSON::Value & node){
	if(!node.isObject()) 
		throw std::runtime_error{"ClusterComponent: Node config not valid: config is not an object."};
	if(!node["id"].isString()) 
		throw std::runtime_error{"ClusterComponent: Node config not valid: the field 'id' is not present or not string."};
	if(!node["processors"].isUndefined()){
		if(!node["processors"].isArray())
			throw std::runtime_error{"ClusterComponent: Node config not valid: the field 'processors' is not an array."};
		for(auto & val : node["processors"].getArray()){
			if(!val.isString())
				throw std::runtime_error{"ClusterComponent: Node config not valid: one entry in 'processors' is not string."};
		}
	}
	if(!node["consumers"].isUndefined()){
		if(!node["consumers"].isArray())
			throw std::runtime_error{"ClusterComponent: Node config not valid: the field 'consumers' is not an array."};
		for(auto & val : node["consumers"].getArray()){
			if(!val.isString())
				throw std::runtime_error{"ClusterComponent: Node config not valid: one entry in 'consumers' is not string."};
		}
	}
	if(!node["forward"].isUndefined()){
		if(!node["forward"].isArray())
			throw std::runtime_error{"ClusterComponent: Node config not valid: the field 'forward' is not an array."};
		for(auto & val : node["forward"].getArray()){
			if(!val.isString())
				throw std::runtime_error{"ClusterComponent: Node config not valid: one entry in 'forward' is not string."};
		}
	}
}

void Susi::ClusterComponent::setupNode(BSON::Value & node){
	LOG(DEBUG) << "configure cluster node: "<<node.toJSON();
	std::string id = node["id"];
	auto apiClient = std::make_shared<Susi::Api::ApiClient>(node["addr"].getString());
	apiClients[id] = apiClient;

	apiClient->registerConsumer("connection::connect",[this,id](Susi::Events::SharedEventPtr evt){
		LOG(INFO) << "ClusterComponent: node " << id <<" is now online";
		auto event = createEvent("cluster::node::open");
		event->payload = id;
		publish(std::move(event));
	});

	apiClient->registerConsumer("connection::close",[this,id](Susi::Events::SharedEventPtr evt){
		LOG(INFO) << "ClusterComponent: node " << id <<" is now offline";
		auto event = createEvent("cluster::node::close");
		event->payload = id;
		publish(std::move(event));
	});

	//forward all events "*@$NODE_ID" to this node.
	registerForwardingForNode(id,"*@"+id);

	if(!node["processors"].isUndefined()){
		for(std::string & topic : node["processors"].getArray()){
			registerProcessorForNode(id,topic);
		}
	}
	if(!node["consumers"].isUndefined()){
		for(std::string & topic : node["consumers"].getArray()){
			registerConsumerForNode(id,topic);
		}
	}
	if(!node["forward"].isUndefined()){
		for(std::string & topic : node["forward"].getArray()){
			registerForwardingForNode(id,topic);
		}
	}
}

void Susi::ClusterComponent::registerProcessorForNode(std::string nodeId, std::string topic){
	LOG(DEBUG) << "setup processor for "<<nodeId<<": "<<topic;
	auto & apiClient = apiClients[nodeId];
	apiClient->registerProcessor(topic,[this,nodeId](Susi::Events::EventPtr remoteEvent){
		if(checkIfInBlacklist(remoteEvent->id)){
			return;
		}
		addToBlacklist(remoteEvent->id);
		struct FinishCallback {
			Susi::Events::EventPtr remoteEvent;
			FinishCallback(Susi::Events::EventPtr evt) : 
				remoteEvent{std::move(evt)} {}
			FinishCallback(FinishCallback && other) : 
				remoteEvent{std::move(other.remoteEvent)} {}
			FinishCallback(FinishCallback & other) : 
				remoteEvent{std::move(other.remoteEvent)} {}
			void operator()(Susi::Events::SharedEventPtr localEvent){
				*remoteEvent = *localEvent;
			}
		};
		auto localEvent = this->createEvent(remoteEvent->topic);
		*localEvent = *remoteEvent;
		FinishCallback finishCallback{std::move(remoteEvent)};
		this->publish(std::move(localEvent),std::move(Susi::Events::Consumer{finishCallback}));
	});
}

void Susi::ClusterComponent::registerConsumerForNode(std::string nodeId, std::string topic){
	LOG(DEBUG) << "setup consumer for "<<nodeId<<": "<<topic;
	auto & apiClient = apiClients[nodeId];
	apiClient->registerConsumer(topic,[this,nodeId](Susi::Events::SharedEventPtr remoteEvent){
		if(checkIfInBlacklist(remoteEvent->id)){
			return;
		}
		addToBlacklist(remoteEvent->id);
		auto localEvent = this->createEvent(remoteEvent->topic);
		*localEvent = *remoteEvent;
		this->publish(std::move(localEvent));
	});
}

void Susi::ClusterComponent::registerForwardingForNode(std::string nodeId, std::string topic){
	LOG(DEBUG) << "setup forwarding to "<<nodeId<<": "<<topic;
	this->subscribe(topic,[this,nodeId,topic](Susi::Events::EventPtr localEvent){
		auto & apiClient = apiClients[nodeId];
		if(!apiClient->isConnected() || checkIfInBlacklist(localEvent->id)){
			return;
		}
		addToBlacklist(localEvent->id);
		LOG(DEBUG) << "Forwarding to "<<nodeId<<": "<<topic;
		struct FinishCallback {
			Susi::Events::EventPtr localEvent;
			FinishCallback(Susi::Events::EventPtr evt) : 
				localEvent{std::move(evt)} {}
			FinishCallback(FinishCallback && other) : 
				localEvent{std::move(other.localEvent)} {}
			FinishCallback(FinishCallback & other) : 
				localEvent{std::move(other.localEvent)} {}
			void operator()(Susi::Events::SharedEventPtr remoteEvent){
				LOG(DEBUG) << "Forwarding finished.";
				if(localEvent){
					*localEvent = *remoteEvent;
				}
			}
		};
		auto remoteEvent = apiClient->createEvent(localEvent->topic);
		*remoteEvent = *localEvent;
		FinishCallback finishCallback{std::move(localEvent)};
		apiClient->publish(std::move(remoteEvent),std::move(finishCallback));
	});
}



