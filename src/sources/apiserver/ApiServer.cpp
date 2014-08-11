#include "apiserver/ApiServer.h"
	
void Susi::Api::ApiServer::onConnect(std::string & id) {}
void Susi::Api::ApiServer::onClose(std::string & id) {
	senders.erase(id);
}
	
void Susi::Api::ApiServer::onMessage(std::string & id, Susi::Util::Any & packet) {
	try{
		auto & type = packet["type"];
		if(type.isString()){
			if(type=="registerConsumer"){
				handleRegisterConsumer(id,packet);
			}else if(type=="registerProcessor"){
				handleRegisterProcessor(id,packet);
			}else if(type=="unregister"){
				handleRegisterConsumer(id,packet);
			}else if(type=="publish"){
				handlePublish(id,packet);
			}else if(type=="ack"){
				handleAck(id,packet);
			}else {
				sendFail(id,"type not known");
			}
		}else{
			sendFail(id,"type is not a string");
		}
	}catch(const std::exception & e){
		std::string msg = "exception while processing: ";
		msg += e.what();
		sendFail(id,msg);
	}
}

void Susi::Api::ApiServer::handleRegisterConsumer(std::string & id, Susi::Util::Any & packet){
	auto & data = packet["data"];
	if(data.isString()){
		std::string topic = data;
		auto & subs = subscriptions[id];
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
			packet["type"] = "event";
			packet["needack"] = false;
			packet["data"] = Susi::Util::Any::Object{
				{"id",(int)event->id},
				{"topic",event->topic},
				{"headers",headers},
				{"payload",event->payload}
			};
			std::string _id = id;
			std::cout<<"got consumer event, try to send it "+packet.toString()<<std::endl;
			send(_id,packet);
		};
		long subid = Susi::Events::subscribe(topic,callback);
		subs[topic] = subid;
		sendOk(id);
	}else{
		sendFail(id,"data is not a string");
	}
}
void Susi::Api::ApiServer::handleRegisterProcessor(std::string & id, Susi::Util::Any & packet){
	auto & data = packet["data"];
	if(data.isString()){
		std::string topic = data;
		auto & subs = subscriptions[id];
		if(subs.find(topic) != subs.end()){
			sendFail(id,"you are allready subscribed to "+topic);
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
		sendFail(id,"data is not a string");
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
			sendFail(id,"you are not registered for "+topic);
		}
	}else{
		sendFail(id,"data is not a string");
	}
}
void Susi::Api::ApiServer::handlePublish(std::string & id, Susi::Util::Any & packet){
	auto & eventData = packet["data"];
	if(!eventData.isObject() || !eventData["topic"].isString()){
		sendFail(id,"data is not an object or topic is not set correctly");
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
	sendOk(id);
}

void Susi::Api::ApiServer::handleAck(std::string & id, Susi::Util::Any & packet){
	auto & eventData = packet["data"];
	if(!eventData.isObject() || !eventData["topic"].isString()){
		sendFail(id,"data is not an object or topic is not set correctly");
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

void Susi::Api::ApiServer::sendOk(std::string & id){
	Susi::Util::Any response;
	response["type"] = "status";
	response["error"] = false;
	send(id,response);
}

void Susi::Api::ApiServer::sendFail(std::string & id,std::string error){
	Susi::Util::Any response;
	response["type"] = "status";
	response["error"] = true;
	response["data"] = error;
	send(id,response);	
}