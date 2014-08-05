#include "events/global.h"
#include "world/World.h"

long Susi::Events::subscribe(std::string topic, Susi::Events::Processor handler){
	return world.eventManager->subscribe(topic,handler);
}
long Susi::Events::subscribe(Susi::Events::Predicate pred, Susi::Events::Processor handler){
	return world.eventManager->subscribe(pred,handler);
}
long Susi::Events::subscribe(std::string topic, Susi::Events::Consumer handler){
	return world.eventManager->subscribe(topic,handler);
}
long Susi::Events::subscribe(Susi::Events::Predicate pred, Susi::Events::Consumer handler){
	return world.eventManager->subscribe(pred,handler);
}
bool Susi::Events::unsubscribe(long id){
	return world.eventManager->unsubscribe(id);
}
void Susi::Events::publish(Susi::Events::EventPtr event, Susi::Events::Consumer finishCallback){
	world.eventManager->publish(std::move(event),std::move(finishCallback));
}
void Susi::Events::ack(Susi::Events::EventPtr event){
	if(world.eventManager.get() != nullptr){
		world.eventManager->ack(std::move(event));
	}
}

void Susi::Events::deleter(Event *event){
	//std::cout<<"calling deleter of "<<event<<std::endl;
	if(event!=nullptr){
			Susi::Events::EventPtr ptr(event,Susi::Events::deleter);
		try{
			ack(std::move(ptr));
		}catch(const std::exception & e){
			std::cout<<"error in deleter:"<<e.what()<<std::endl;
		}
	}
}

Susi::Events::EventPtr Susi::Events::createEvent(std::string topic){
	auto event = Susi::Events::EventPtr{new Event,deleter};
	long id = std::chrono::system_clock::now().time_since_epoch().count();
	event->id = id;
	event->topic = topic;
	return event;
}