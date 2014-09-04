#include "apiserver/ApiServerForComponent.h"
#include "world/World.h"

void Susi::Api::ApiServerForComponent::onClose(std::string & id) {
	std::cout<<"lost connection..."<<std::endl;
	world.sessionManager->killSession(id);
	senders.erase(id);
	eventsToAck.erase(id);

	auto & subs = consumerSubscriptions[id];
	for(auto & kv : subs) {
		eventManager->unsubscribe(kv.second);
	}
	consumerSubscriptions.erase(id);
	auto & subs2 = processorSubscriptions[id];
	for(auto & kv : subs2) {
		eventManager->unsubscribe(kv.second);
	}
	processorSubscriptions.erase(id);
}

void Susi::Api::ApiServerForComponent::handleRegisterConsumer(std::string & id, Susi::Util::Any & packet){
	auto & data = packet["data"];

	if(data.isString()){
		std::string topic = data;
		auto & subs = consumerSubscriptions[id];
		if(subs.find(topic) != subs.end()){
			sendFail(id,"you are allready subscribed to "+topic);
			return;
		}
		Susi::Events::Consumer callback = [this,id](Susi::Events::SharedEventPtr event){
			Susi::Util::Any::Array headers;
			for(auto & kv : event->headers){
				headers.push_back(Susi::Util::Any::Object{{kv.first,kv.second}});
			}
			Susi::Util::Any packet;
			packet["type"] = "consumerEvent";
			packet["data"] = Susi::Util::Any::Object{
				{"id",event->id},
				{"topic",event->topic},
				{"headers",headers},
				{"payload",event->payload},
				{"sessionid",event->sessionID}
			};
			std::string _id = id;
			//std::cout<<"got consumer event, try to send it "+packet.toString()<<std::endl;
			send(_id,packet);
		};
		long subid = eventManager->subscribe(topic,callback);
		subs[topic] = subid;
		sendOk(id);
	}else{
		std::cout<<"handleRegisterConsumer-> data is not a string:"<<std::endl;
		sendFail(id,"data is not a string");
	}
}
void Susi::Api::ApiServerForComponent::handleRegisterProcessor(std::string & id, Susi::Util::Any & packet){
	auto & data = packet["data"];
	if(data.isString()){
		std::string topic = data;
		auto & subs = processorSubscriptions[id];
		if(subs.find(topic) != subs.end()){
			sendFail(id,"you are allready subscribed to "+topic);
			return;
		}
		long subid = eventManager->subscribe(topic,[this,id](Susi::Events::EventPtr event){
			Susi::Util::Any::Array headers;
			for(auto & kv : event->headers){
				headers.push_back(Susi::Util::Any::Object{{kv.first,kv.second}});
			}
			Susi::Util::Any packet;
			packet["type"] = "processorEvent";
			packet["data"] = Susi::Util::Any::Object{
				{"id",event->id},
				{"topic",event->topic},
				{"headers",headers},
				{"payload",event->payload},
				{"sessionid",event->sessionID}
			};
			std::string _id = id;
			eventsToAck[_id][event->id] = std::move(event);
			send(_id,packet);
		});
		subs[topic] = subid;
		sendOk(id);
	}else{
		sendFail(id,"data is not a string");
	}
}
void Susi::Api::ApiServerForComponent::handleUnregisterConsumer(std::string & id, Susi::Util::Any & packet){
	auto & data = packet["data"];
	if(data.isString()){
		std::string topic = data;
		auto & subs = consumerSubscriptions[id];
		if(subs.find(topic)!=subs.end()){
			long subid = subs[topic];
			eventManager->unsubscribe(subid);
			sendOk(id);
		}else{
			sendFail(id,"you are not registered for "+topic);
		}
	}else{
		sendFail(id,"data is not a string");
	}
}

void Susi::Api::ApiServerForComponent::handleUnregisterProcessor(std::string & id, Susi::Util::Any & packet){
	auto & data = packet["data"];
	if(data.isString()){
		std::string topic = data;
		auto & subs = processorSubscriptions[id];
		if(subs.find(topic)!=subs.end()){
			long subid = subs[topic];
			eventManager->unsubscribe(subid);
			sendOk(id);
		}else{
			sendFail(id,"you are not registered for "+topic);
		}
	}else{
		sendFail(id,"data is not a string");
	}
}
void Susi::Api::ApiServerForComponent::handlePublish(std::string & id, Susi::Util::Any & packet){
	auto & eventData = packet["data"];
	if(!eventData.isObject() || !eventData["topic"].isString()){
		sendFail(id,"data is not an object or topic is not set correctly");
		return;
	}
	auto event = eventManager->createEvent(eventData["topic"]);
	event->sessionID = id;
	auto eventID = eventData["id"];
	if(eventID.isNull()){
		eventID = std::chrono::system_clock::now().time_since_epoch().count();
	}

	event->id = eventID;

	if(eventData["headers"].isArray()){
		Susi::Util::Any::Array arr = eventData["headers"];
		for(Susi::Util::Any::Object & val : arr){
			for(auto & kv : val){
				event->headers.push_back(std::make_pair(kv.first,(std::string)kv.second));
			}
		}
	}
	if(!eventData["payload"].isNull()){
		event->payload = eventData["payload"];
	}

	eventManager->publish(std::move(event),[this,id](Susi::Events::SharedEventPtr event){
		Susi::Util::Any::Array headers;
		for(auto & kv : event->headers){
			headers.push_back(Susi::Util::Any::Object{{kv.first,kv.second}});
		}
		Susi::Util::Any packet;
		packet["type"] = "ack";
		packet["data"] = Susi::Util::Any::Object{
			{"id",event->id},
			{"topic",event->topic},
			{"headers",headers},
			{"payload",event->payload},
			{"sessionid",event->sessionID}
		};
		std::string _id = id;
		send(_id,packet);
	});

	sendOk(id);
}

void Susi::Api::ApiServerForComponent::handleAck(std::string & id, Susi::Util::Any & packet){
	auto & eventData = packet["data"];
	if(!eventData.isObject() || !eventData["topic"].isString()){
		sendFail(id,"data is not an object or topic is not set correctly");
		return;
	}
	long eventID = eventData["id"];
	auto event = std::move(eventsToAck[id][eventID]);
	eventsToAck[id].erase(eventID);
	event->headers.clear();
	event->id = eventID;
	if(eventData["headers"].isArray()){
		Susi::Util::Any::Array arr = eventData["headers"];
		for(Susi::Util::Any::Object & val : arr){
			for(auto & kv : val){
				event->headers.push_back(std::make_pair(kv.first,(std::string)kv.second));
			}
		}
	}
	if(!eventData["payload"].isNull()){
		event->payload = eventData["payload"];
	}
	eventManager->ack(std::move(event));
	sendOk(id);
}
