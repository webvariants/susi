#include "susi/LocalEventSystem.h"

void Susi::LocalEventSystem::registerProcessor(const std::string & topic, Processor processor){
	std::uint64_t id = generateId();
	processors[id] = {topic,processor};
}

uint64_t Susi::LocalEventSystem::registerConsumer(const std::string & topic, Consumer consumer){
	std::uint64_t id = generateId();
	consumers[id] = {topic,consumer};
}

void Susi::LocalEventSystem::publish(ProcessorEvent && event){
	std::string & id = (*event)["id"].getString();
	std::string & topic = (*event)["topic"].getString();
	publishProcesses.emplace(id,PublishProcess{});
	PublishProcess & process = publishProcesses[id];
	for(auto & kv : processors){
		for(auto & kv : processors){
			std::regex reg{kv.second.first};
			if(std::regex_match(topic,reg)){
				process.processors.emplace_back(kv.second.second);
			}
		}
		for(auto & kv : consumers){
			std::regex reg{kv.second.first};
			if(std::regex_match(topic,reg)){
				process.consumers.emplace_back(kv.second.second);
			}
		}
		ack(std::move(event));
	}
}

void Susi::LocalEventSystem::ack(ProcessorEvent && event){
	std::string & id = (*event)["id"].getString();
	
}

	protected:
		// id -> pair<topic,processor>
		std::map<std::uint64_t,std::pair<std::string,Processor>> processors
		// id -> pair<topic,consumer>
		std::map<std::uint64_t,std::pair<std::string,Consumer>> consumers;
		
		struct PublishProcess {
			std::vector<Processor&> processors;
			std::vector<Consumer&> consumers;
			size_t nextProcessor = 0;
		};

	};

}