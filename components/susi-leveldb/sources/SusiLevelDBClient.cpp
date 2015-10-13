#include "susi/SusiLevelDBClient.h"

SusiLevelDBClient::SusiLevelDBClient(std::string addr,short port, std::string key, std::string cert, std::string dbPath) :
  susi_{new Susi::SusiClient{addr,port,key,cert}},
  dbPath_{dbPath} {
	dbOptions_.create_if_missing = true;
	leveldb::DB *dbPtr;
	leveldb::DB::Open(dbOptions_, dbPath_, &dbPtr);
	db_.reset(dbPtr);
	susi_->registerProcessor("leveldb::put",[this](Susi::EventPtr event){
		handlePut(std::move(event));
	});
	susi_->registerProcessor("leveldb::get",[this](Susi::EventPtr event){
		handleGet(std::move(event));
	});
	susi_->registerProcessor("leveldb::delete",[this](Susi::EventPtr event){
		handleDelete(std::move(event));
	});
}

void SusiLevelDBClient::join(){
	susi_->join();
}

bool SusiLevelDBClient::validateFieldsForPut(const Susi::EventPtr & event){
	if(!(event->payload.isObject()) ||
	   !(event->payload["key"].isString()) ||
	   event->payload["value"].isUndefined()){
		return false;
	}
	return true;
}

bool SusiLevelDBClient::validateFieldsForGet(const Susi::EventPtr & event){
	if(!(event->payload.isObject()) ||
	   !(event->payload["key"].isString())){
		return false;
	}
	return true;
}

void SusiLevelDBClient::handlePut(Susi::EventPtr event){
	if(!validateFieldsForPut(event)){
		event->headers.push_back({"error","leveldb put error"});
		return;
	}
	leveldb::Status s = db_.get()->Put(leveldb::WriteOptions(), event->payload["key"].getString(), event->payload["value"].toJSON());
	if(!s.ok()){
		event->headers.push_back({"error","leveldb put error: "+s.ToString()});
		return;
	}
	event->payload["success"] = true;
}

void SusiLevelDBClient::handleGet(Susi::EventPtr event){
	if(!validateFieldsForGet(event)){
		event->headers.push_back({"error","leveldb get error"});
		return;
	}
	std::string valueStr;
	leveldb::Status s = db_.get()->Get(leveldb::ReadOptions(), event->payload["key"].getString(), &valueStr);
	if(!s.ok()){
		event->headers.push_back({"error","leveldb get error: "+s.ToString()});
		return;
	}
	event->payload["value"] = BSON::Value::fromJSON(valueStr);
}

void SusiLevelDBClient::handleDelete(Susi::EventPtr event){
	if(!validateFieldsForGet(event)){
		event->headers.push_back({"error","leveldb delete error"});
		return;
	}
	leveldb::Status s = db_.get()->Delete(leveldb::WriteOptions(), event->payload["key"].getString());
	if(!s.ok()){
		event->headers.push_back({"error","leveldb delete error: "+s.ToString()});
		return;
	}
	event->payload["success"] = true;
}

