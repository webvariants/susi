#include "susi/StateFileComponent.h"
#include <fstream>

Susi::StateFileComponent::StateFileComponent(Susi::SusiClient & susi, BSON::Value & config) :
  susi_{susi},
  stateFile_{config["file"].getString()} {
    readStateFile();
	susi_.registerProcessor("(statefile|state)::put",[this](Susi::EventPtr event){
		handlePut(std::move(event));
	});
	susi_.registerProcessor("(statefile|state)::get",[this](Susi::EventPtr event){
		handleGet(std::move(event));
	});
	susi_.registerProcessor("(statefile|state)::delete",[this](Susi::EventPtr event){
		handleDelete(std::move(event));
	});
}

void Susi::StateFileComponent::join(){
	susi_.join();
}

bool Susi::StateFileComponent::validateFieldsForPut(const Susi::EventPtr & event){
	if(!(event->payload.isObject()) ||
	   !(event->payload["key"].isString()) ||
	   event->payload["value"].isUndefined()){
		return false;
	}
	return true;
}

bool Susi::StateFileComponent::validateFieldsForGet(const Susi::EventPtr & event){
	if(!(event->payload.isObject()) ||
	   !(event->payload["key"].isString())){
		return false;
	}
	return true;
}

void Susi::StateFileComponent::handlePut(Susi::EventPtr event){
	if(!validateFieldsForPut(event)){
		event->headers.push_back({"Error","statefile put error"});
		return;
	}
    state_[event->payload["key"].getString()] = event->payload["value"];
	event->payload["success"] = writeStateFile();
}

void Susi::StateFileComponent::handleGet(Susi::EventPtr event){
	if(!validateFieldsForGet(event)){
		event->headers.push_back({"Error","statefile get error"});
		return;
	}
    BSON::Value value = state_[event->payload["key"].getString()];
	event->payload["value"] = value;
}

void Susi::StateFileComponent::handleDelete(Susi::EventPtr event){
	if(!validateFieldsForGet(event)){
		event->headers.push_back({"Error","statefile delete error"});
		return;
	}
    state_[event->payload["key"].getString()].reset();
	event->payload["success"] = writeStateFile();
}


bool Susi::StateFileComponent::readStateFile(){
    std::ifstream file{stateFile_};
    if(file.is_open()){
        std::string content;
        std::string line;
        while(std::getline(file,line)){
            content += line + "\n";
        }
        state_ = BSON::Value::fromJSON(content);
        if(!state_.isUndefined()){
            file.close();
            return true;
        }
    }
    file.close();
    return false;
}

bool Susi::StateFileComponent::writeStateFile(){
    std::ofstream file{stateFile_};
    file << state_.toJSON();
    file.close();
    return true;
}
