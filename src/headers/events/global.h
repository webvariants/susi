/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 * 
 * @author: Tino Rusch (tino.rusch@webvariants.de)
 */

#ifndef __EVENTSGLOBAL__
#define __EVENTSGLOBAL__

#include "world/World.h"

namespace Susi {
	namespace Events {
		long subscribe(std::string topic, Handler handler){
			return world.eventManager->subscribe(topic,handler);
		}
		long subscribe(Predicate pred, Handler handler){
			return world.eventManager->subscribe(pred,handler);
		}
		bool unsubscribe(long id){
			return world.eventManager->unsubscribe(id);
		}
		void publish(EventPtr event, Handler finishCallback){
			world.eventManager->publish(std::move(event),std::move(finishCallback));
		}
		void ack(EventPtr event){
			if(world.eventManager.get() != nullptr){
				world.eventManager->ack(std::move(event));
			}
		}
		
		void deleter(Event *event){
			//std::cout<<"calling deleter of "<<event<<std::endl;
			if(event!=nullptr){
				EventPtr ptr(event,deleter);
				try{
					ack(std::move(ptr));
				}catch(const std::exception & e){
					std::cout<<"error in deleter:"<<e.what()<<std::endl;
				}
			}
		}

		EventPtr createEvent(std::string topic){
			auto event = EventPtr{new Event,deleter};
			long id = std::chrono::system_clock::now().time_since_epoch().count();
			event->id = id;
			event->topic = topic;
			return event;
		}



	}	
}



#endif // __EVENTSGLOBAL__