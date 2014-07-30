#include "db/Manager.h"

Susi::DB::Manager::Manager() {}

Susi::DB::Manager::Manager(Poco::Dynamic::Var config) {

	std::vector<std::tuple<std::string,std::string,std::string>> dbs;
	if(config.isVector()) {
		dbs = config.extract<std::vector<std::tuple<std::string,std::string,std::string>>>();

		for(std::size_t i=0; i<dbs.size(); ++i){
  	  		this->addDatabase(std::get<0>(dbs[i]), std::get<1>(dbs[i]),std::get<2>(dbs[i]));
		}	
	}
}

Susi::DB::Manager::Manager(std::vector<std::tuple<std::string,std::string,std::string>> dbs) {
	for(std::size_t i=0; i<dbs.size(); ++i){
  	  this->addDatabase(std::get<0>(dbs[i]), std::get<1>(dbs[i]),std::get<2>(dbs[i]));
	}	
	std::string msg = "initialized database manager with ";
	msg += std::to_string(dbs.size());
	msg += " databases";
	Susi::info(msg);
}

void Susi::DB::Manager::init(Poco::Dynamic::Var config) {
	std::vector<std::tuple<std::string,std::string,std::string>> dbs;
	if(config.isVector()) {
		dbs = config.extract<std::vector<std::tuple<std::string,std::string,std::string>>>();

		for(std::size_t i=0; i<dbs.size(); ++i){
  	  		addDatabase(std::get<0>(dbs[i]), std::get<1>(dbs[i]),std::get<2>(dbs[i]));
		}	
	}
}

void Susi::DB::Manager::addDatabase(std::string identifier,std::string dbtype,std::string connectURI) {
	this->dbMap[identifier] = Susi::DB::ManagerItem{dbtype,connectURI};
}

std::shared_ptr<Susi::DB::Database> Susi::DB::Manager::getDatabase(std::string identifier) {
	if(this->dbMap.count(identifier) > 0) {
		Susi::DB::ManagerItem item = this->dbMap[identifier];
		
		if(item.connected == false) {
			item.db = std::shared_ptr<Susi::DB::Database>(new Susi::DB::Database(item.dbtype, item.connectURI));
			item.connected = true;
		}

		return item.db;
		
	} else {
		std::string msg = "DB not found: ";
		msg += identifier;
		Susi::error(msg);
	}

	return nullptr;
}
