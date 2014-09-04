#include "events/Manager.h"

long Susi::Events::Manager::subscribe(std::string topic, Susi::Events::Processor processor){
	std::lock_guard<std::mutex> lock(mutex);
	long id = std::chrono::system_clock::now().time_since_epoch().count();
	processorsByTopic[topic].push_back(std::make_pair(id,processor));
	return id;
}
long Susi::Events::Manager::subscribe(Susi::Events::Predicate pred, Susi::Events::Processor processor){
	std::lock_guard<std::mutex> lock(mutex);
	long id = std::chrono::system_clock::now().time_since_epoch().count();
	processorsByPred.push_back(std::make_tuple(id,pred,processor));
	return id;
}
long Susi::Events::Manager::subscribe(std::string topic, Susi::Events::Consumer consumer){
	std::lock_guard<std::mutex> lock(mutex);
	long id = std::chrono::system_clock::now().time_since_epoch().count();
	consumersByTopic[topic].push_back(std::make_pair(id,consumer));
	return id;
}
long Susi::Events::Manager::subscribe(Susi::Events::Predicate pred, Susi::Events::Consumer consumer){
	std::lock_guard<std::mutex> lock(mutex);
	long id = std::chrono::system_clock::now().time_since_epoch().count();
	consumersByPred.push_back(std::make_tuple(id,pred,consumer));
	return id;
}
bool Susi::Events::Manager::unsubscribe(long id){
	std::lock_guard<std::mutex> lock(mutex);
	for(auto & kv : processorsByTopic){
		auto & processors = kv.second;
		for(size_t i=0;i<processors.size();i++){
			if(processors[i].first == id){
				processors.erase(processors.begin()+i);
				return true;
			}
		}
	}
	for(size_t i=0;i<processorsByPred.size();i++){
		auto & tuple = processorsByPred[i];
		if(std::get<0>(tuple) == id){
			processorsByPred.erase(processorsByPred.begin()+i);
			return true;
		}
	}
	for(auto & kv : consumersByTopic){
		auto & consumers = kv.second;
		for(size_t i=0;i<consumers.size();i++){
			if(consumers[i].first == id){
				consumers.erase(consumers.begin()+i);
				return true;
			}
		}
	}
	for(size_t i=0;i<consumersByPred.size();i++){
		auto & tuple = consumersByPred[i];
		if(std::get<0>(tuple) == id){
			consumersByPred.erase(consumersByPred.begin()+i);
			return true;
		}
	}
	for(auto & kv : publishProcesses){
		for(auto & processorPair : kv.second->processors){
			if(processorPair.first == id){
				processorPair.second = Processor{};
				break;
			}
		}
		for(auto & consumerPair : kv.second->consumers){
			if(consumerPair.first == id){
				consumerPair.second = Consumer{};
				break;
			}
		}
	}
	return false;
}

// public publish api function
void Susi::Events::Manager::publish(Susi::Events::EventPtr event, Susi::Events::Consumer finishCallback){
	if(event.get()==nullptr){
		//std::cout<<"event is nullptr"<<std::endl;
		event.release();
		return;
	}
	{
		std::lock_guard<std::mutex> lock(mutex);
		auto process = std::make_shared<PublishProcess>();
		for(auto & kv : processorsByTopic){
			if(kv.first == event->topic){
				for(auto & pair : kv.second){
					process->processors.push_back(pair);
				}
				break;
			}
		}
		for(auto & tuple : processorsByPred){
			if((std::get<1>(tuple))(*event)){
				process->processors.push_back(std::make_pair(std::get<0>(tuple),std::get<2>(tuple)));
			}
		}

		for(auto & kv : consumersByTopic){
			if(kv.first == event->topic){
				for(auto & pair : kv.second){
					process->consumers.push_back(pair);
				}
				break;
			}
		}
		for(auto & tuple : consumersByPred){
			if((std::get<1>(tuple))(*event)){
				process->consumers.push_back(std::make_pair(std::get<0>(tuple),std::get<2>(tuple)));
			}
		}
		process->consumers.push_back(std::pair<long,Consumer>{0,finishCallback});
		publishProcesses[event->id] = process;
	} // release lock
	ack(std::move(event));
}

// pass event back to system
void Susi::Events::Manager::ack(EventPtr event){
	if(event.get()==nullptr){
		event.release();
		return;
	}
	struct Work {
		EventPtr event;
		Manager *manager;
		Work(EventPtr evt, Manager *mgr) : event{std::move(evt)}, manager{mgr} {}
		Work(Work && other) : event{std::move(other.event)}, manager{other.manager} {}
		Work(Work & other) : event{std::move(other.event)}, manager{other.manager} {}
		void operator()(){
			long id = event->id;
			std::shared_ptr<PublishProcess> process;
			{
				std::unique_lock<std::mutex> lock(manager->mutex);
				for(auto & kv : manager->publishProcesses){
					if(kv.first == id){
						process = kv.second;
						break;
					}
				}
			}
			if(process.get()==nullptr){
				std::cout<<"cant find process, this should not happen"<<std::endl;
				std::cout<<event->topic<<std::endl;
				delete event.release();
				return;
			}
			//std::cout<<"ack event "<<event->topic<<std::endl;

			std::unique_lock<std::mutex> lock(process->mutex);
			while(process->errors.size() > 0){
				event->headers.push_back(std::make_pair("error",process->errors.back()));
				process->errors.pop_back();
			}
			if(process->current < process->processors.size()){
				try{
					auto nextProcessor = process->processors[process->current++].second;
					if(nextProcessor){
						nextProcessor(std::move(event));
					}
				}catch(const std::exception & e){
					process->errors.push_back(e.what());
				}
			}else{
				std::shared_ptr<Event> sharedEvent{event.release()};
				for(auto & consumerPair : process->consumers){
					auto consumer = consumerPair.second;
					if(consumer) manager->pool.add([consumer,sharedEvent](){
						consumer(sharedEvent);
					});
				}
				{
					std::unique_lock<std::mutex> lock(manager->mutex);
					for(auto & kv : manager->publishProcesses){
						if(kv.second == process) {
							manager->publishProcesses.erase(kv.first);
							break;
						}
					}
				}
				manager->publishFinished.notify_one();
			}
		}
	};

	long id = event->id;
	auto error = [id,this](std::string msg){
		std::unique_lock<std::mutex> lock(mutex);
		std::shared_ptr<PublishProcess> process;
		for(auto & kv : publishProcesses){
			if(kv.first == id){
				process = kv.second;
			}
		}
		if(process.get()!=nullptr){
			process->errors.push_back(msg);
		}
	};
	Work work{std::move(event),this};
	pool.add(std::move(work),error);
}

Susi::Events::EventPtr Susi::Events::Manager::createEvent(std::string topic){
	auto event = Susi::Events::EventPtr{new Susi::Events::Event{topic},[this](Event * event){
		this->deleter(event);
	}};
	return event;
}

void Susi::Events::Manager::deleter(Event *event){
	//std::cout<<"calling deleter of "<<event<<std::endl;
	if(event!=nullptr){
		Susi::Events::EventPtr ptr(event,[this](Event *event){
			deleter(event);
		});
		try{
			ack(std::move(ptr));
		}catch(const std::exception & e){
			std::cout<<"error in deleter:"<<e.what()<<std::endl;
		}
	}
}
