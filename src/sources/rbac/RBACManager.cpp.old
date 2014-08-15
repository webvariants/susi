#include "rbac/RBACManager.h"

Susi::RBACManager::RBACManager(){}

Susi::RBACManager::RBACManager(std::string fileLocation){
	loadFile(fileLocation);
}

void Susi::RBACManager::setRead(std::string object, std::string role, bool read){
	auto & roleList = _acl[object];
	bool found = false;
	for(auto & entry : roleList) {
		if(entry.role == role){
			entry.read = read;
			found = true;
		}
	}
	if(!found){
		roleList.push_back(Susi::RBACManager::Entry{role,read,defaultWrite});
	}
}

bool Susi::RBACManager::getRead(std::string object, std::string role){
	auto & roleList = _acl[object];
	for(auto & entry : roleList) {
		if(entry.role == role){
			return entry.read;
		}
	}
	return defaultRead;
}

void Susi::RBACManager::setWrite(std::string object, std::string role, bool write){
	auto & roleList = _acl[object];
	bool found = false;
	for(auto & entry : roleList) {
		if(entry.role == role){
			entry.write = write;
			found = true;
		}
	}
	if(!found){
		roleList.push_back(Susi::RBACManager::Entry{role,defaultRead,write});
	}
}

bool Susi::RBACManager::getWrite(std::string object, std::string role){
	auto & roleList = _acl[object];
	for(auto & entry : roleList) {
		if(entry.role == role){
			return entry.write;
		}
	}
	return defaultWrite;
}

void Susi::RBACManager::setDefaultRead(bool read){
	defaultRead = read;
}
void Susi::RBACManager::setDefaultWrite(bool write){
	defaultWrite = write;
}

bool Susi::RBACManager::loadFile(std::string fileLocation){
	Susi::IOController io;
	std::string fileContent = io.readFile(fileLocation);
	Poco::JSON::Parser parser;
	Poco::Dynamic::Var data;
	try{
		data = parser.parse(fileContent);
	}catch(const std::exception & e){
		std::string msg = "cannot parse rbac file: ";
		msg += e.what();
		Susi::error(msg);
		return false;
	}
	Poco::JSON::Object::Ptr object = data.extract<Poco::JSON::Object::Ptr>();
	std::map<std::string,std::vector<Susi::RBACManager::Entry>> acl;
	std::vector<std::string> names;
	object->getNames(names);
	for(auto & name : names) {
		Poco::JSON::Array roleList = object->get(name).extract<Poco::JSON::Array>();
		std::vector<Susi::RBACManager::Entry> entries;
		for(auto & entry : roleList) {
			entries.push_back(Susi::RBACManager::Entry{
				entry["role"],
				entry["read"],
				entry["write"]
			});
		}
		acl[name] = entries;
	}
	_acl = acl;
	return true;
}

bool Susi::RBACManager::saveFile(std::string fileLocation){
	Poco::Dynamic::Struct<std::string> obj;
	for(auto & kv : _acl){
		Poco::Dynamic::Vector list;
		for(auto & entry : kv.second){
			list.push_back(Susi::Event::Payload({
				{"role",entry.role},
				{"read",entry.read},
				{"write",entry.write}
			}));
		}
		obj[kv.first] = list;
	}
	std::stringstream ss;
	Poco::JSON::Stringifier::stringify(obj,ss,0);
	std::string json = ss.str();
	Susi::IOController io;
	return (io.writeFile(fileLocation,json)>0);
}