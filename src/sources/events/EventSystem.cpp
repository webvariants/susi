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

#include "events/EventSystem.h"
#include "world/World.h"
 
bool testIfGlob(std::string str){
	if(str.find("*") != std::string::npos ||
	   str.find("[") != std::string::npos ||
	   str.find("?") != std::string::npos) {
		return true;
	}
	return false;
}

long Susi::EventSystem::subscribe(std::string topic,std::function<void(Event&)> callback){
	std::lock_guard<std::mutex> lock(mutex);
	Poco::Timestamp stamp;
	long id = stamp.epochMicroseconds();
	auto & m = callbacks[topic];
	m[id] = callback;
	return id;
}
void Susi::EventSystem::unsubscribe(std::string topic,long id){
	std::lock_guard<std::mutex> lock(mutex);
	auto & m = callbacks[topic];
	m.erase(id);
}

long Susi::EventSystem::subscribe_glob(std::string topic,std::function<void(Event&)> callback){
	std::lock_guard<std::mutex> lock(mutex);
	Poco::Timestamp stamp;
	long id = stamp.epochMicroseconds();
	auto & m = globs[topic];
	m[id] = callback;
	return id;
}

void Susi::EventSystem::unsubscribe_glob(std::string topic,long id){
	std::lock_guard<std::mutex> lock(mutex);
	auto & m = globs[topic];
	m.erase(id);
}

void Susi::EventSystem::publish(Event & event){
	std::vector<std::function<void(Event&)>> calls;
	{//collect callbacks guarded by mutex but execute without
		std::lock_guard<std::mutex> lock(mutex);
		for(auto & kv : globs){
			Poco::Glob glob(kv.first);
			auto & callbacks = kv.second;
			if (glob.match(event.topic)) {
				for(auto & kv : callbacks){
					calls.push_back(kv.second);
				}
			}
		}
		auto & m = callbacks[event.topic];
		for(auto & kv : m){
			calls.push_back(kv.second);
		}
	}//release lock
	for(auto & call : calls){
		call(event);
	}
}

long Susi::subscribe(std::string topic,std::function<void(Event&)> callback){
	if (testIfGlob(topic)){
		return world.eventSystem->subscribe_glob(topic,callback);
	}
	return world.eventSystem->subscribe(topic,callback);
}

void Susi::publish(Event & event){
	world.eventSystem->publish(event);
}

void Susi::publish(Event & event, std::function<void(Susi::Event&)> callback){
	std::string returnAddr = std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
	once(returnAddr, callback);
	event.returnAddr = returnAddr;
	world.eventSystem->publish(event);
}

void Susi::unsubscribe(std::string topic,long id){
	if (testIfGlob(topic)){
		world.eventSystem->unsubscribe_glob(topic,id);
	}
	world.eventSystem->unsubscribe(topic,id);
}

void Susi::answerEvent(Event &request, Poco::Dynamic::Var response){
	if(request.returnAddr != ""){
		Susi::Event event(request.returnAddr,response);
		Susi::publish(event);
	}
}

void Susi::once(std::string topic,std::function<void(Susi::Event&)> callback){
	std::shared_ptr<long> id(new long);
	*id = Susi::subscribe(topic,[topic,id,callback](Susi::Event& event){
		Susi::unsubscribe(topic,*id);
		callback(event);
	});
}

Poco::Dynamic::Var Susi::request(std::string topic,
		Poco::Dynamic::Var payload,
		std::chrono::duration<int,std::milli> timeout){
	std::string returnAddr = std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
	Poco::Dynamic::Var result;
	bool callbackCalled = false;
	std::condition_variable cond;
	std::mutex m;
	Susi::once(returnAddr,[&result,&cond,&callbackCalled](Susi::Event & event){
		result = event.payload;
		callbackCalled = true;
		cond.notify_all();
	});
	Susi::Event request(topic);
	if(!payload.isEmpty()){
		request.payload = payload;
	}
	request.returnAddr = returnAddr;
	Susi::publish(request);
	std::unique_lock<std::mutex> lk(m);
	cond.wait_for(lk,timeout,[&callbackCalled](){return callbackCalled;});
	return result;
}

Poco::Dynamic::Var Susi::request(std::string topic){
	std::chrono::duration<int,std::milli> dur{250};
	return Susi::request(topic,Poco::Dynamic::Var{},dur);
}
