#include "enginestarter/EngineStarter.h"

Susi::EngineStarter::Starter::Starter(std::string path) {
	this->path = path;
}

void Susi::EngineStarter::Starter::killall() {
	try{
		for(auto & ph : phs){
			Poco::Process::kill(ph);
		}
	}catch(const std::exception & e){
		std::string msg = "Error in EngineStarter::killall : ";
		msg += e.what();
		Susi::Logger::error(msg);
	}

}

void Susi::EngineStarter::Starter::execute() {
	try{
		this->rec_dir(this->path);
	}catch(const std::exception & e){
		std::string msg = "Error in EngineStarter::execute : ";
		msg += e.what();
		Susi::Logger::error(msg);
	}
}

void Susi::EngineStarter::Starter::rec_dir(const std::string & path)
{
  Poco::DirectoryIterator end;
  for (Poco::DirectoryIterator it(path); it != end; ++it) {
  	if(!it->isDirectory() && it->canExecute()) {
  		std::vector<std::string> args;
		Poco::ProcessHandle ph = Poco::Process::launch(it->path(), args);
		this->phs.push_back(ph);
	}else if (it->isDirectory()) {
		rec_dir(it->path());
	}
  }
}