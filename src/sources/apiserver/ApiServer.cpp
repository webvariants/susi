#include "apiserver/ApiServer.h"

	
void Susi::Api::ApiServer::onMessage(std::string & id, Susi::Util::Any & packet) {
	auto & type = packet["type"];
	if(type.isString()){
		if(type=="registerConsumer"){
			handleRegisterConsumer(id,packet);
		}else if(type=="registerProcessor"){
			handleRegisterProcessor(id,packet);
		}else if(type=="unregister"){
			handleRegisterConsumer(id,packet);
		}else if(type=="publish"){
			handleRegisterConsumer(id,packet);
		}else if(type=="ack"){
			handleAck(id,packet);
		}else {
			handleMalformedPacket(id,packet);
		}
	}else{
		handleMalformedPacket(id,packet);
	}
}

void Susi::Api::ApiServer::handleRegisterConsumer(std::string & id, Susi::Util::Any & packet){
	auto & data = packet["data"];
	if(data.isString()){
		std::string topic = data;
		auto & subs = subscriptions[id];
		if(subs.find(topic) != subs.end()){
			sendFail(id);
			return;
		}
		Susi::Events::Consumer callback = [this,id](Susi::Events::SharedEventPtr event){
			Susi::Util::Any::Array headers;
			for(auto & kv : event->headers){
				headers.push_back(Susi::Util::Any::Object{{kv.first,kv.second}});
			}
			Susi::Util::Any packet;
			packet["type"] = "event";
			packet["needack"] = true;
			packet["data"] = Susi::Util::Any::Object{
				{"id",(int)event->id},
				{"topic",event->topic},
				{"headers",headers},
				{"payload",event->payload}
			};
			std::string _id = id;
			send(_id,packet);
		};
		long subid = Susi::Events::subscribe(topic,callback);
		subs[topic] = subid;
		sendOk(id);
	}else{
		handleMalformedPacket(id,packet);
	}
}
void Susi::Api::ApiServer::handleRegisterProcessor(std::string & id, Susi::Util::Any & packet){
	auto & data = packet["data"];
	if(data.isString()){
		std::string topic = data;
		auto & subs = subscriptions[id];
		if(subs.find(topic) != subs.end()){
			sendFail(id);
			return;
		}
		long subid = Susi::Events::subscribe(topic,[this,id](Susi::Events::EventPtr event){
			Susi::Util::Any::Array headers;
			for(auto & kv : event->headers){
				headers.push_back(Susi::Util::Any::Object{{kv.first,kv.second}});
			}
			Susi::Util::Any packet;
			packet["type"] = "event";
			packet["needack"] = true;
			packet["data"] = Susi::Util::Any::Object{
				{"id",(int)event->id},
				{"topic",event->topic},
				{"headers",headers},
				{"payload",event->payload}
			};
			std::string _id = id;
			eventsToAck[_id][event->id] = std::move(event);
			send(_id,packet);
		});
		subs[topic] = subid;
		sendOk(id);
	}else{
		handleMalformedPacket(id,packet);
	}
}
void Susi::Api::ApiServer::handleUnregister(std::string & id, Susi::Util::Any & packet){
	auto & data = packet["data"];
	if(data.isString()){
		std::string topic = data;
		auto & subs = subscriptions[id];
		if(subs.find(topic)!=subs.end()){
			long subid = subs[topic];
			Susi::Events::unsubscribe(subid);
			sendOk(id);
		}else{
			sendFail(id);
		}
	}else{
		handleMalformedPacket(id,packet);
	}
}
void Susi::Api::ApiServer::handlePublish(std::string & id, Susi::Util::Any & packet){
	auto & eventData = packet["data"];
	if(!eventData.isObject() || !eventData["topic"].isString()){
		sendFail(id);
		return;
	}
	auto event = Susi::Events::createEvent(eventData["topic"]);
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
	Susi::Events::publish(std::move(event),[this,id](Susi::Events::SharedEventPtr event){
		Susi::Util::Any::Array headers;
		for(auto & kv : event->headers){
			headers.push_back(Susi::Util::Any::Object{{kv.first,kv.second}});
		}
		Susi::Util::Any packet;
		packet["type"] = "event";
		packet["data"] = Susi::Util::Any::Object{
			{"id",(int)event->id},
			{"topic",event->topic},
			{"headers",headers},
			{"payload",event->payload}
		};
		std::string _id = id;
		send(_id,packet);
	});
}

void Susi::Api::ApiServer::handleAck(std::string & id, Susi::Util::Any & packet){
	auto & eventData = packet["data"];
	if(!eventData.isObject() || !eventData["topic"].isString()){
		sendFail(id);
		return;
	}
	long eventID = (int)eventData["id"];
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
	Susi::Events::ack(std::move(event));
	sendOk(id);
}

void Susi::Api::ApiServer::handleMalformedPacket(std::string & id, Susi::Util::Any & packet){
	sendFail(id);
}

void Susi::Api::ApiServer::sendOk(std::string & id){
	Susi::Util::Any response;
	response["type"] = "status";
	response["error"] = false;
	send(id,response);
}

void Susi::Api::ApiServer::sendFail(std::string & id){
	Susi::Util::Any response;
	response["type"] = "status";
	response["error"] = true;
	send(id,response);	
}