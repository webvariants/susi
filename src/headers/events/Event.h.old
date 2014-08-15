/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 * 
 * @author: Tino Rusch (tino.rusch@webvariants.de)
 */

#ifndef __EVENT__
#define __EVENT__

#include <Poco/Dynamic/Var.h> 
#include <Poco/Dynamic/Struct.h>
#include <map> 

namespace Susi {

class Event {
public:
	std::string topic;
	Poco::Dynamic::Var payload;
	std::string sessionID;
	std::string returnAddr;
	Event(std::string topic) 
		: topic{topic} {}
	Event(std::string topic,Poco::Dynamic::Var payload) 
		: topic{topic}, 
		  payload{payload},
		  sessionID{""},
		  returnAddr{""} {}
	Event(std::string topic,Poco::Dynamic::Var payload,std::string sessionID) 
		: topic{topic}, 
		  payload{payload},
		  sessionID{sessionID},
		  returnAddr{""} {}
	Event(std::string topic,Poco::Dynamic::Var payload,std::string sessionID,std::string returnAddr) 
		: topic{topic}, 
		  payload{payload},
		  sessionID{sessionID},
		  returnAddr{returnAddr} {}
	std::string toString(){
		Poco::JSON::Object result;
		result.set("topic",topic);
		if(returnAddr != ""){
			result.set("returnaddr",returnAddr);
		}
		if(sessionID != ""){
			result.set("sessionid",sessionID);
		}
		if (!payload.isEmpty()){
			result.set("payload",payload);
		}
		std::stringstream ss;
		result.stringify(ss);
		return ss.str();
	}

	static Poco::Dynamic::Var Payload(std::map<std::string,Poco::Dynamic::Var> initList){
		Poco::Dynamic::Struct<std::string> payload(initList);
		return payload;
	}
};

}

#endif // __EVENT__
