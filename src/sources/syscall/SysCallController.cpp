#include "syscall/SysCallController.h"

Susi::Syscall::Controller::Controller(std::string configPath) {
	IOController io;
	try{
		std::string configData = io.readFile(configPath);

		if(configData != ""){

			std::map<std::string,Susi::Util::Any> data = Susi::Util::Any::fromString(configData);		

			for (std::map<std::string,Susi::Util::Any>::iterator it=data.begin(); it!=data.end(); ++it) {
				//std::cout << it->first << " => " << it->second.toString() <<std::endl;

				std::string cmd  = it->second["cmd"];
				std::string args = it->second["args"];
				bool background  = it->second["background"];

				this->processMap[it->first] = Susi::Syscall::ProcessItem{cmd,args,background}; 
			}
		}
	}catch(...){
		Susi::Logger::error("config is malformed");
	}
}

bool Susi::Syscall::Controller::startProcess(std::string process_type, std::map<std::string, std::string> argsReplace) {

	auto searchItem = this->processMap.find(process_type);
	if(searchItem != this->processMap.end()) {

		Susi::Syscall::Worker worker(process_type, searchItem->second.cmd, searchItem->second.getArgs(argsReplace), searchItem->second.background);		

		pool.start(worker);
	    pool.joinAll();

		return true;
	} else {		
		return false;
	}
}