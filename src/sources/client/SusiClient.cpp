#include "client/SusiClient.h"

void Susi::SusiClient::publish(std::string topic, Poco::Dynamic::Var payload = Poco::Dynamic::Var{}, std::string returnAddr = ""){
	auto data = Susi::Event::Payload({
		{"type","publish"},
		{"key",topic},
		{"payload",payload}
	});
	if(returnAddr!=""){
		data["returnaddr"] = returnAddr;
	}
	conn.send(data);
}

bool Susi::SusiClient::subscribe(std::string topic, std::function<void(Susi::Event&)> callback){
	auto data = Susi::Event::Payload({
		{"type","subscribe"},
		{"key",topic}
	});
	conn.send(data);
	subscriptions[topic] = callback;
	return true;
}

bool Susi::SusiClient::unsubscribe(std::string topic){
	auto data = Susi::Event::Payload({
		{"type","unsubscribe"},
		{"key",topic}
	});
	conn.send(data);
	subscriptions.erase(topic);
	return true;
}

void Susi::SusiClient::getEvent(){
	Poco::Dynamic::Var packet;
	while(true){
		if(!conn.recv(packet)){
			break;
		}
		auto typeVar = Susi::Util::getFromVar(packet,"type");
		std::string type = typeVar.convert<std::string>();
		if (type == "event"){
			auto keyVar = Susi::Util::getFromVar(packet,"key");
			std::string key = keyVar.convert<std::string>();
			auto payloadVar = Susi::Util::getFromVar(packet,"payload");
			auto returnVar = Susi::Util::getFromVar(packet,"returnaddr");
			Susi::Event event(key,payloadVar);
			if(!returnVar.isEmpty())event.returnAddr = returnVar.convert<std::string>();
			subscriptions[key](event);
			break;
		}
	}
}
