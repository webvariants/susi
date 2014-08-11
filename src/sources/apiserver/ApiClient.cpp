#include "apiserver/ApiClient.h"

void onMessage(Susi::Util::Any & message) {
	auto & type = message["type"];
	if(type.isString()){

	}
}

void Susi::Api::ApiClient::publish(Susi::Events::EventPtr event, Susi::Events::Consumer finishHandler){

}
void Susi::Api::ApiClient::subscribe(std::string topic, Susi::Events::Processor processor){

}
void Susi::Api::ApiClient::subscribe(std::string topic, Susi::Events::Consumer consumer){

}

void Susi::Api::ApiClient::handleEvent(Susi::Events::EventPtr event){

}
void Susi::Api::ApiClient::handleAck(Susi::Events::EventPtr event){

}
void Susi::Api::ApiClient::handleStatus(bool error,std::string message){

}