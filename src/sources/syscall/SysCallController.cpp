#include "syscall/SysCallController.h"

Susi::Syscall::Controller::Controller(std::string configPath) {
	IOController io;
	std::string configData = io.readFile(configPath);

	if(configData != ""){
		Poco::JSON::Parser parser;
		Poco::Dynamic::Var var = parser.parse(configData);
		Poco::JSON::Object::Ptr object = var.extract<Poco::JSON::Object::Ptr>();

		std::vector<std::string> names;
		object->getNames(names);
		for(auto & name : names){			
			Poco::JSON::Object::Ptr obj = object->getObject(name);
			this->processMap[name] = Susi::Syscall::ProcessItem{obj->get("cmd").toString(),obj->get("args").toString(),(obj->get("background").toString() == "false") ? false : true};
		}
	}
}

bool Susi::Syscall::Controller::startProcess(std::string returnAddr, std::string process_type, std::map<std::string, std::string> argsReplace) {

	auto searchItem = this->processMap.find(process_type);
	if(searchItem != this->processMap.end()) {

		Susi::Syscall::Worker worker(returnAddr, searchItem->second.cmd, searchItem->second.getArgs(argsReplace), searchItem->second.background);
			
		pool.start(worker);
	    pool.joinAll();

		return true;
	} else {
		return false;
	}
}