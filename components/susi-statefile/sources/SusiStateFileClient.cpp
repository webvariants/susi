#include "susi/SusiStateFileClient.h"
#include <fstream>

SusiStateFileClient::SusiStateFileClient(std::string addr,short port, std::string key, std::string cert, std::string stateFile) :
  susi_{new Susi::SusiClient{addr,port,key,cert}},
  stateFile_{stateFile} {
    readStateFile();
	susi_->registerProcessor("(statefile|state)::put",[this](Susi::EventPtr event){
		handlePut(std::move(event));
	});
	susi_->registerProcessor("(statefile|state)::get",[this](Susi::EventPtr event){
		handleGet(std::move(event));
	});
	susi_->registerProcessor("(statefile|state)::delete",[this](Susi::EventPtr event){
		handleDelete(std::move(event));
	});
}

void SusiStateFileClient::join(){
	susi_->join();
}

bool SusiStateFileClient::validateFieldsForPut(const Susi::EventPtr & event){
	if(!(event->payload.isObject()) ||
	   !(event->payload["key"].isString()) ||
	   event->payload["value"].isUndefined()){
		return false;
	}
	return true;
}

bool SusiStateFileClient::validateFieldsForGet(const Susi::EventPtr & event){
	if(!(event->payload.isObject()) ||
	   !(event->payload["key"].isString())){
		return false;
	}
	return true;
}

void SusiStateFileClient::handlePut(Susi::EventPtr event){
	if(!validateFieldsForPut(event)){
		event->headers.push_back({"Error","statefile put error"});
		return;
	}
    state_[event->payload["key"].getString()] = event->payload["value"];
	event->payload["success"] = writeStateFile();
}

void SusiStateFileClient::handleGet(Susi::EventPtr event){
	if(!validateFieldsForGet(event)){
		event->headers.push_back({"Error","statefile get error"});
		return;
	}
    BSON::Value value = state_[event->payload["key"].getString()];
	event->payload["value"] = value;
}

void SusiStateFileClient::handleDelete(Susi::EventPtr event){
	if(!validateFieldsForGet(event)){
		event->headers.push_back({"Error","statefile delete error"});
		return;
	}
    state_[event->payload["key"].getString()].reset();
	event->payload["success"] = writeStateFile();
}


bool SusiStateFileClient::readStateFile(){
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

bool SusiStateFileClient::writeStateFile(){
    std::ofstream file{stateFile_};
    file << state_.toJSON();
    file.close();
    return true;
}
