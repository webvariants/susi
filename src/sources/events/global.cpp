#include "events/global.h"
#include "world/World.h"

long Susi::Events::subscribe(std::string topic, Susi::Events::Processor handler){
	if(world.eventManager.get()==nullptr)return 0;
	return world.eventManager->subscribe(topic,handler);
}
long Susi::Events::subscribe(Susi::Events::Predicate pred, Susi::Events::Processor handler){
	if(world.eventManager.get()==nullptr)return 0;
	return world.eventManager->subscribe(pred,handler);
}
long Susi::Events::subscribe(std::string topic, Susi::Events::Consumer handler){
	if(world.eventManager.get()==nullptr)return 0;
	return world.eventManager->subscribe(topic,handler);
}
long Susi::Events::subscribe(Susi::Events::Predicate pred, Susi::Events::Consumer handler){
	if(world.eventManager.get()==nullptr)return 0;
	return world.eventManager->subscribe(pred,handler);
}
bool Susi::Events::unsubscribe(long id){
	if(world.eventManager.get()==nullptr)return 0;
	return world.eventManager->unsubscribe(id);
}
void Susi::Events::publish(Susi::Events::EventPtr event, Susi::Events::Consumer finishCallback){
	if(world.eventManager.get()==nullptr){
		delete event.release();
		return;
	}
	world.eventManager->publish(std::move(event),std::move(finishCallback));
}
void Susi::Events::ack(Susi::Events::EventPtr event){
	if(world.eventManager.get()==nullptr){
		delete event.release();
		return;
	}
	world.eventManager->ack(std::move(event));	
}
Susi::Events::EventPtr Susi::Events::createEvent(std::string topic){
	if(world.eventManager.get()==nullptr){
		throw std::runtime_error{"no event manager"};
	}
	return world.eventManager->createEvent(topic);
}