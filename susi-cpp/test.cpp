#include "susi/EventManager.h"

#include <chrono>

int main(){

	auto eventsystem = std::make_shared<Susi::EventManager>();
	eventsystem->registerProcessor("test-topic", [eventsystem](Susi::EventPtr event){
		std::cout<<"processor: "<<event->topic<<std::endl;
	});
	eventsystem->registerConsumer("test-topic", [](Susi::SharedEventPtr event){
		std::cout<<"consumer1: "<<event->topic<<std::endl;
	});
	eventsystem->registerConsumer("test-topic", [](Susi::SharedEventPtr event){
		std::cout<<"consumer2: "<<event->topic<<std::endl;
	});
	eventsystem->registerConsumer("test-topic", [](Susi::SharedEventPtr event){
		std::cout<<"consumer3: "<<event->topic<<std::endl;
	});
	auto event = eventsystem->createEvent("test-topic");
	eventsystem->publish(std::move(event),[](Susi::SharedEventPtr event){
		std::cout<<"finish callback: "<<event->topic<<std::endl;
	});

	eventsystem->join();

	return 0;
}