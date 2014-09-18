#include "apiserver/ApiServerComponent.h"

void Susi::Api::ApiServerComponent::onConnect(std::string & id){
	Susi::Logger::info("got new connection!");
	sessionManager->updateSession(id);
}

void Susi::Api::ApiServerComponent::onClose(std::string & id) {
	Susi::Logger::info("lost connection...");
	sessionManager->killSession(id);
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

void Susi::Api::ApiServerComponent::onMessage(std::string & id, Susi::Util::Any & packet) {
	try{
		//std::cout<<"onMessage:"<<packet.toJSONString()<<std::endl;
		auto type = packet["type"];
		if(type.isString()){
			if(type=="registerConsumer"){
				handleRegisterConsumer(id,packet);
			}else if(type=="registerProcessor"){
				handleRegisterProcessor(id,packet);
			}else if(type=="unregisterConsumer"){
				handleUnregisterConsumer(id,packet);
			}else if(type=="unregisterProcessor"){
				handleUnregisterProcessor(id,packet);
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

void Susi::Api::ApiServerComponent::handleRegisterConsumer(std::string & id, Susi::Util::Any & packet){
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
			//std::cout<<"got consumer event, try to send it "+packet.toJSONString()<<std::endl;
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
void Susi::Api::ApiServerComponent::handleRegisterProcessor(std::string & id, Susi::Util::Any & packet){
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
void Susi::Api::ApiServerComponent::handleUnregisterConsumer(std::string & id, Susi::Util::Any & packet){
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

void Susi::Api::ApiServerComponent::handleUnregisterProcessor(std::string & id, Susi::Util::Any & packet){
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
void Susi::Api::ApiServerComponent::handlePublish(std::string & id, Susi::Util::Any & packet){
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

void Susi::Api::ApiServerComponent::handleAck(std::string & id, Susi::Util::Any & packet){
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

void Susi::Api::ApiServerComponent::sendOk(std::string & id){
	Susi::Util::Any response;
	response["type"] = "status";
	response["error"] = false;
	send(id,response);
}

void Susi::Api::ApiServerComponent::sendFail(std::string & id,std::string error){
	Susi::Util::Any response;
	response["type"] = "status";
	response["error"] = true;
	response["data"] = error;
	send(id,response);
}