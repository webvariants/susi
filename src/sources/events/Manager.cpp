#include "events/Manager.h"

using namespace Susi::Events;

Manager::Manager(int threadCount, int chanBuffSize) {
	//std::cout<<"init manager"<<std::endl;
	workChannel = std::shared_ptr<Susi::Util::Channel<EventPtr>>(new Susi::Util::Channel<EventPtr>(chanBuffSize));
	for(int i=0;i<threadCount;i++){
		std::thread t([this](){
			Manager::AcknowledgeWorker worker(this,workChannel);
			 try{
				worker.run();
			}catch(const std::exception & e){
				//std::cout<<"error in worker: "<<e.what()<<std::endl;
			}
		});
		workers.push_back(std::move(t));
	}
}

void Manager::AcknowledgeWorker::run(){
	while(true){
		auto event = _workChannel->get();
		//std::cout<<"got event in worker: "<<event.get()<<std::endl;
		if(event.get() == nullptr){
			//std::cout<<"got nullptr in ack!"<<std::endl;	
			continue;
		}
		std::shared_ptr<PublishProcess> process;
		{
			std::lock_guard<std::mutex> lock(_manager->publishProcessesMutex);
			if(_manager->publishProcesses.count(event->id)==0){
				//std::cout<<"cant find PublishProcess in ack, release event."<<std::endl;
				Event * evt = event.release();
				delete evt;
				continue;
			}
			process = _manager->publishProcesses[event->id];
		}
		while(process->errors.size()>0){
			event->headers.push_back(std::make_pair("error",process->errors[process->errors.size()-1]));
			process->errors.pop_back();
		}
		try{
			if(process->current == process->handlers.size()){
				long id = event->id;
				{
					std::lock_guard<std::mutex> lock(_manager->publishProcessesMutex);
					_manager->publishProcesses.erase(id);
					//std::cout<<"deleted publish process "<<id<<std::endl;
				}
				//std::cout<<"calling finish handler with event "<<event.get()<<std::endl;
				process->finishHandler(std::move(event));
				//std::cout<<"finish handler done."<<std::endl;
				continue;
				
			}else{
				//std::cout<<"calling handler no. "<<process->current<<std::endl;
				process->handlers[process->current++](std::move(event));
				//std::cout<<"handler done."<<std::endl;
			}
		}catch(const std::exception & e){
			//std::cout<<"got exception in handler "<<process->current-1<<std::endl;
			//std::cout<<e.what()<<std::endl;
			process->errors.push_back(e.what());
		}	
	}
}

long Manager::subscribe(std::string topic, Handler handler){
	long id = std::chrono::system_clock::now().time_since_epoch().count();
	auto & handlers = handlersByTopic[topic];
	handlers.push_back(std::make_pair(id,handler));
	return id;
}

long Manager::subscribe(Predicate pred, Handler handler){
	long id = std::chrono::system_clock::now().time_since_epoch().count();
	handlersByPred.push_back(std::make_tuple(id,pred,handler));
	return id;
}

bool Manager::unsubscribe(long id){
	for(auto & kv : handlersByTopic){
		for(auto it = kv.second.begin(); it != kv.second.end(); ++it){
			if(it->first == id){
				kv.second.erase(it);
				return true;
			}
		}
	}
	for(auto it = handlersByPred.begin(); it != handlersByPred.end(); ++it){
		if(std::get<0>(*it) == id){
			handlersByPred.erase(it);
			return true;
		}
	}
	return false;
}

void Manager::publish(EventPtr event, Handler finishCallback){
	//std::cout<<"calling publish"<<std::endl;
	long id = event->id;
	auto process = std::shared_ptr<PublishProcess>{new PublishProcess};
	process->finishHandler = finishCallback;
	{
		std::lock_guard<std::mutex> lock(handlersByTopicMutex);
		for ( auto & kv : handlersByTopic ) {
			if (kv.first == event->topic) {
				for (auto & handlerpair : kv.second) {
					process->handlers.push_back(std::get<1>(handlerpair));
				}
			}
		}
	}
	{
		std::lock_guard<std::mutex> lock(handlersByPredMutex);
		for ( auto & tuple : handlersByPred ) {
			if (std::get<1>(tuple)(*event)) {
				process->handlers.push_back(std::get<2>(tuple));
			}
		}
	}
	process->current = 0;
	{
		std::lock_guard<std::mutex> lock(publishProcessesMutex);
		publishProcesses[id] = process;
	}
	this->ack(std::move(event));
}

void Manager::ack(EventPtr event){
	//std::cout<<"calling ack with "<<event.get()<<std::endl;
	workChannel->put(std::move(event));
}

