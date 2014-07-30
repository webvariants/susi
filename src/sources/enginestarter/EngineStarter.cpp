#include "enginestarter/EngineStarter.h"

Susi::EngineStarter::EngineStarter(std::string path) {
	this->path = path;
	Susi::subscribe("enginestarter::start",[this](Susi::Event & event){
		this->execute();
	});
	Susi::subscribe("enginestarter::restart",[this](Susi::Event & event){
		this->killall();
		this->execute();
	});
	Susi::subscribe("enginestarter::stop",[this](Susi::Event & event){
		this->killall();
	});
	Susi::subscribe("global::start",[this](Susi::Event & event){
		this->execute();
	});
	Susi::subscribe("global::restart",[this](Susi::Event & event){
		this->killall();
		this->execute();
	});
	Susi::subscribe("global::stop",[this](Susi::Event & event){
		this->killall();
	});
}

void Susi::EngineStarter::killall() {
	try{
		for(auto & ph : phs){
			Poco::Process::kill(ph);
		}
	}catch(const std::exception & e){
		std::string msg = "Error in EngineStarter::killall : ";
		msg += e.what();
		Susi::error(msg);
	}

}

void Susi::EngineStarter::execute() {
	try{
		this->rec_dir(this->path);
	}catch(const std::exception & e){
		std::string msg = "Error in EngineStarter::execute : ";
		msg += e.what();
		Susi::error(msg);
	}
}

void Susi::EngineStarter::rec_dir(const std::string & path)
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