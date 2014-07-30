#include "db/EventInterface.h"
#include "world/World.h"

void Susi::DB::handleQuery(Susi::Event & event){
	try{
		/*
		std::string type = event.payload["type"];
		std::string uri = event.payload["uri"];
		std::string query = event.payload["query"];
		Susi::Database db(type,uri);
		auto res = db.query(query);
		Susi::Event resp(event.returnAddr,res);
		Susi::publish(resp);
		*/
		std::string identifier = event.payload["identifier"];
		std::string query = event.payload["query"];

		//std::cout<<identifier<<" "<<query<<std::endl;
		auto db = world.dbManager->getDatabase(identifier);
		if(db==nullptr){
			Susi::Event resp(event.returnAddr,Susi::Event::Payload({{"error",true}}));
			Susi::publish(resp);
		}else{
			auto res = db->query(query);
			Susi::Event resp(event.returnAddr,res);
			Susi::publish(resp);
		}

	}catch(const std::exception & e){
		std::string msg = "Error in handleQuery(): ";
		msg += e.what();
		Susi::error(msg);
	}
}

void Susi::DB::init(){
	subscribe("db::query",Susi::DB::handleQuery);
}