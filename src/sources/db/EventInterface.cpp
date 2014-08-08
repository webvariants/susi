#include "db/EventInterface.h"
#include "world/World.h"

void Susi::DB::init(){
	Susi::Events::subscribe("db::query",Susi::DB::handleQuery);
}

void Susi::DB::handleQuery(Susi::Events::EventPtr  event){
	try{		
		std::string identifier = event->payload["identifier"];
		std::string query = event->payload["query"];
		
		auto db = world.dbManager->getDatabase(identifier);
		if(db==nullptr){
			event->payload["error"] = true;			
		}else{
			auto res = db->query(query);
			event->payload["result"] = res;			
		}

	}catch(const std::exception & e){
		std::string msg = "Error in handleQuery(): ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}