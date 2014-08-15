#include "apiserver/ApiClient.h"

void Susi::Api::ApiClient::publish(Susi::Events::EventPtr event, Susi::Events::Consumer finishCallback){
	finishCallbacks[event->getID()] = finishCallback;
	sendPublish(*event);
	delete event.release();
}

long Susi::Api::ApiClient::subscribe(std::string topic, Susi::Events::Processor processor){
	sendRegisterProcessor(topic);
	return Susi::Events::Manager::subscribe(topic,processor);
}

long Susi::Api::ApiClient::subscribe(std::string topic, Susi::Events::Consumer consumer){
	sendRegisterConsumer(topic);
	return Susi::Events::Manager::subscribe(topic,consumer);
}


void Susi::Api::ApiClient::onConsumerEvent(Susi::Events::Event & event){
	auto evt = createEvent(event.getTopic());
	*evt = event;
	Susi::Events::Manager::publish(std::move(evt));
}

void Susi::Api::ApiClient::onProcessorEvent(Susi::Events::Event & event){
	auto evt = createEvent(event.getTopic());
	*evt = event;
	Susi::Events::Manager::publish(std::move(evt),[this](Susi::Events::SharedEventPtr event){
		sendAck(*event);
	});
}

void Susi::Api::ApiClient::onAck(Susi::Events::Event & event){
	auto evt = std::make_shared<Susi::Events::Event>(event);
	auto callback = finishCallbacks[evt->getID()];
	if(callback){
		callback(evt);
	}
}