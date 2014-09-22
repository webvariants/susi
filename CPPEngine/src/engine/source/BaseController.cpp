#include "BaseController.h"

Susi::Cpp::BaseController::BaseController()  {
	this.engine = Susi::Cpp::Engine::getInstance();
}

void Susi::Cpp::BaseController::log(Susi::Util::Any any) {
	Susi::Logger::log(any);
}
void Susi::Cpp::BaseController::publish(Susi::Events::EventPtr event, Susi::Events::Consumer finishedCallback) {
	engine->publish(std::move(event, finishedCallback));
}
long Susi::Cpp::BaseController::subscribe(std::string topic, Susi::Events::Consumer c) {
	return engine->subscribe(topic, c);
}
long Susi::Cpp::BaseController::subscribe(std::string topic, Susi::Events::Processor p) {
	return engine->subscribe(topic, p);
}
void Susi::Cpp::BaseController::acknowledge(Susi::Events::EventPtr event) {
	engine->acknowledge(event);
}
void Susi::Cpp::BaseController::unsubscribe(std::string topic) {
	log("Cpp Engine BaseController unsubscribe not yet implemented!");
}
Susi::Events::EventPtr Susi::Cpp::BaseController::createEvent(std::string topic, Susi::Util::Any payload) {
	Susi::Events::EventPtr event = engine->createEvent(topic);
	if(!payload.isNull()) {
		event->setPayload(payload);
	}
	return std::move(event);
}
