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

#ifndef __JSONSERVER__
#define __JSONSERVER__

#include <map>
#include <sstream>
#include "util/Any.h"
#include "apiserver/ServerInterfaces.h"

namespace Susi{
namespace Api{

template<class DataServer>
class JSONServer : public Susi::Api::MessageServer, public DataServer {
protected:
	std::map<std::string,std::tuple<std::stringstream,int,int>> buckets;
	virtual void onData(std::string & id, std::string & data) override {
		auto & bucket = std::get<0>(buckets[id]);
		auto & opening = std::get<1>(buckets[id]);
		auto & closing = std::get<2>(buckets[id]);
		for(size_t i=0;i<data.size();i++){
			char c = data[i];
			bucket << c;
			if(c=='{')opening++;
			if(c=='}'){
				closing++;
				if(opening==closing){
					std::string msg = bucket.str();
					bucket.str("");
					opening=0;
					closing=0;
					Susi::Util::Any var;
					try{
						var = Susi::Util::Any::fromString(msg);
					}catch(const std::exception &e ){
						onError(id);
						continue;
					}
					onMessage(id,var);
				}
			}
		}
	}
public:
	JSONServer(unsigned short port) : DataServer(port) {}
	
	virtual void onMessage(std::string & id, Susi::Util::Any & data) = 0;
	virtual void onError(std::string & id){}

	void send(std::string & id, Susi::Util::Any & data){
		std::string str = data.toString();
		DataServer::send(id,str);
	}

	void close(std::string & id){
		DataServer::close(id);
	}

	void run(){
		DataServer::run();
	}

	void join(){
		DataServer::join();
	}

	void stop(){
		DataServer::stop();
	}
};

}
}

#endif // __JSONSERVER__