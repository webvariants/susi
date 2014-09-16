#include "util/VarHelper.h"

Poco::Dynamic::Var Susi::Util::getFromVar(Poco::Dynamic::Var & var, std::string key){
	try{
		auto m = var.extract<Poco::JSON::Object::Ptr>();
		return m->get(key);
	}catch(const std::exception & e){
		std::cout<<"failed to get var from "<<var.toString()<<" ("<<key<<")"<<std::endl;
		return Poco::Dynamic::Var();
	}
}
