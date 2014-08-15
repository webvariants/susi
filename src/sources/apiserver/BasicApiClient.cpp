#include "apiserver/BasicApiClient.h"

void Susi::Api::BasicApiClient::sendPublish(Susi::Events::Event & event){
	Susi::Util::Any packedEvent = event.toAny();
	Susi::Util::Any packet = Susi::Util::Any::Object{
		{"type","publish"},
		{"data",packedEvent}
	};

	JSONTCPClient::send(packet);
}

void Susi::Api::BasicApiClient::sendAck(Susi::Events::Event & event){
	Susi::Util::Any packedEvent = event.toAny();
	Susi::Util::Any packet = Susi::Util::Any::Object{
		{"type","ack"},
		{"data",packedEvent}
	};
	//std::cout<<"send ack"<<std::endl;
	JSONTCPClient::send(packet);
	//std::cout<<"sended ack"<<std::endl;
}

void Susi::Api::BasicApiClient::sendRegisterConsumer(std::string topic){
	Susi::Util::Any packet = Susi::Util::Any::Object{
		{"type","registerConsumer"},
		{"data",topic}
	};
	JSONTCPClient::send(packet);	
}

void Susi::Api::BasicApiClient::sendRegisterProcessor(std::string topic){
	Susi::Util::Any packet = Susi::Util::Any::Object{
		{"type","registerProcessor"},
		{"data",topic}
	};
	JSONTCPClient::send(packet);	
}

void Susi::Api::BasicApiClient::onMessage(Susi::Util::Any & message){
	//std::cout<<"got message in basic api client"<<std::endl;
	std::string type = message["type"];
	if(type=="ack"){
		Susi::Events::Event event{message["data"]};		
		onAck(event);
	}else if(type=="consumerEvent"){
		Susi::Events::Event event{message["data"]};
		onConsumerEvent(event);
	}else if(type=="processorEvent"){
		Susi::Events::Event event{message["data"]};
		onProcessorEvent(event);
	}else{
		//std::cout<<"got status"<<std::endl;
	}
}
