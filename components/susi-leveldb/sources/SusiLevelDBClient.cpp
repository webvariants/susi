#include "susi/LevelDBComponent.h"

Susi::LevelDBComponent::LevelDBComponent(Susi::SusiClient & susi, std::string dbPath) :
  susi_{susi},
  dbPath_{dbPath} {
	dbOptions_.create_if_missing = true;
	leveldb::DB *dbPtr;
	leveldb::DB::Open(dbOptions_, dbPath_, &dbPtr);
	db_.reset(dbPtr);
	susi_.registerProcessor("(leveldb|state)::put",[this](Susi::EventPtr event){
		handlePut(std::move(event));
	});
	susi_.registerProcessor("(leveldb|state)::get",[this](Susi::EventPtr event){
		handleGet(std::move(event));
	});
	susi_.registerProcessor("(leveldb|state)::delete",[this](Susi::EventPtr event){
		handleDelete(std::move(event));
	});
    susi_.registerProcessor("leveldb::getPrefix",[this](Susi::EventPtr event){
        handleGetPrefix(std::move(event));
    });
}

void Susi::LevelDBComponent::join(){
	susi_.join();
}

bool Susi::LevelDBComponent::validateFieldsForPut(const Susi::EventPtr & event){
	if(!(event->payload.isObject()) ||
	   !(event->payload["key"].isString()) ||
	   event->payload["value"].isUndefined()){
		return false;
	}
	return true;
}

bool Susi::LevelDBComponent::validateFieldsForGet(const Susi::EventPtr & event){
	if(!(event->payload.isObject()) ||
	   !(event->payload["key"].isString())){
		return false;
	}
	return true;
}

void Susi::LevelDBComponent::handlePut(Susi::EventPtr event){
	if(!validateFieldsForPut(event)){
		event->headers.push_back({"Error","leveldb put error"});
		return;
	}
	leveldb::Status s = db_.get()->Put(leveldb::WriteOptions(), event->payload["key"].getString(), event->payload["value"].toJSON());
	if(!s.ok()){
		event->headers.push_back({"Error","leveldb put error: "+s.ToString()});
		return;
	}
    std::cout << "wrote "<<event->payload["key"].getString()<<" to disk."<<std::endl;
	event->payload["success"] = true;
}

void Susi::LevelDBComponent::handleGet(Susi::EventPtr event){
	if(!validateFieldsForGet(event)){
		event->headers.push_back({"Error","leveldb get error"});
		return;
	}
	std::string valueStr;
	leveldb::Status s = db_.get()->Get(leveldb::ReadOptions(), event->payload["key"].getString(), &valueStr);
	if(!s.ok()){
		event->headers.push_back({"Error","leveldb get error: "+s.ToString()});
		return;
	}
    std::cout << "got "<<event->payload["key"].getString()<<" from disk."<<std::endl;
	event->payload["value"] = BSON::Value::fromJSON(valueStr);
}

void Susi::LevelDBComponent::handleDelete(Susi::EventPtr event){
	if(!validateFieldsForGet(event)){
		event->headers.push_back({"Error","leveldb delete error"});
		return;
	}
	leveldb::Status s = db_.get()->Delete(leveldb::WriteOptions(), event->payload["key"].getString());
	if(!s.ok()){
		event->headers.push_back({"Error","leveldb delete error: "+s.ToString()});
		return;
	}
    std::cout << "deleted "<<event->payload["key"].getString()<<" from disk."<<std::endl;
	event->payload["success"] = true;
}

void Susi::LevelDBComponent::handleGetPrefix(Susi::EventPtr event){
	if(!validateFieldsForGet(event)){
		event->headers.push_back({"Error","leveldb get error"});
		return;
	}
    BSON::Value result;
    auto prefix = event->payload["key"].getString();
    std::shared_ptr<leveldb::Iterator> dbIter(db_->NewIterator(leveldb::ReadOptions()));
    for(dbIter->Seek(prefix); dbIter->Valid() && dbIter->key().starts_with(prefix) ; dbIter->Next()){
        if( !dbIter->value().empty() ) {
            auto key = dbIter->key();
            auto value = dbIter->value();
            std::string keyString{key.data(),key.size()};
            std::string valueString{value.data(),value.size()};
            result[keyString] = BSON::Value::fromJSON(valueString);
        }
    }
    std::cout << "got all keys with prefix "<<prefix<<" from disk."<<std::endl;
	event->payload["value"] = result;
}
