/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 *
 * @author: Christian Sonderfeld (christian.sonderfeld@webvariants.de), Tino Rusch (tino.rusch@webvariants.de)
 */


#ifndef __CPPENGINE_BASECONTROLLER__
#define __CPPENGINE_BASECONTROLLER__

#include "events/Event.h"
#include "util/Any.h"
#include "logger/Logger.h"
#include "apiserver/ApiClient.h"

namespace Susi {
namespace Cpp {


class BaseController {
protected:
	std::shared_ptr<Susi::Api::ApiClient> api;
	std::vector<long> subscriptions;

	void log(Susi::Util::Any any){
        Susi::Logger::log(any.toJSONString());
    }
	
	long subscribe( std::string topic, Susi::Events::Processor processor, char authlevel=0) {
        long id = api->subscribe( topic,std::move( processor ),authlevel );
        subscriptions.push_back(id);
        return id;
    }
    
    long subscribe( std::string topic, Susi::Events::Consumer consumer, char authlevel=0) {
        long id = api->subscribe( topic,std::move( consumer ),authlevel );
        subscriptions.push_back(id);
        return id;
    }
    
    void publish( Susi::Events::EventPtr event, Susi::Events::Consumer finishCallback = Susi::Events::Consumer {} ) {
        api->publish( std::move( event ),finishCallback );
    }

    Susi::Events::EventPtr createEvent( std::string topic ) {
        return api->createEvent( topic );
    }

	void unsubscribeAll(){
        for(auto & sub : subscriptions){
            api->unsubscribe(sub);
        }
    }

public:
	BaseController() {}

	void setApi(std::shared_ptr<Susi::Api::ApiClient> api_) {
		api = api_;
	}

	virtual void start() = 0;
	virtual void stop(){
		unsubscribeAll();
	}

	virtual ~BaseController() {}
};


}
}
#endif // __CPPENGINE_BASECONTROLLER__
