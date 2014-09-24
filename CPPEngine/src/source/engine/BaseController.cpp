#include "controller/BaseController.h"

void Susi::Cpp::BaseController::log(Susi::Util::Any any) {
	Susi::Logger::log(any);
}
void Susi::Cpp::BaseController::publish(Susi::Events::EventPtr event, Susi::Events::Consumer finishedCallback) {
	api->publish(std::move(event, finishedCallback));
}
long Susi::Cpp::BaseController::subscribe(std::string topic, Susi::Events::Consumer c) {
	return api->subscribe(topic, c);
}
long Susi::Cpp::BaseController::subscribe(std::string topic, Susi::Events::Processor p) {
	return api->subscribe(topic, p);
}
void Susi::Cpp::BaseController::acknowledge(Susi::Events::EventPtr event) {
	api->acknowledge(event);
}
void Susi::Cpp::BaseController::unsubscribeAll() {
	for(auto sub : subsciptions) {
		api->unsubscribe(sub);
	}
}
Susi::Events::EventPtr Susi::Cpp::BaseController::createEvent(std::string topic, Susi::Util::Any payload) {
	Susi::Events::EventPtr event = api->createEvent(topic);
	if(!payload.isNull()) {
		event->setPayload(payload);
	}
	return std::move(event);
}
