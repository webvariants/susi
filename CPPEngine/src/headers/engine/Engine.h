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


#ifndef __CPPENGINE_ENGINE__
#define __CPPENGINE_ENGINE__

#include <memory>
#include <map>

#include "apiserver/ApiClient.h"
#include "BaseController.h"

namespace Susi {
namespace Cpp {


class Engine {
protected:

	std::shared_ptr<Susi::Api::ApiClient> susi_api;

	std::map<std::string, std::shared_ptr<Susi::Cpp::BaseController>> controller;

	void addController(std::string name, Susi::Cpp::BaseController *c) {
		controller[name] = std::shared_ptr<Susi::Cpp::BaseController>{c};
		controller[name]->setApi(susi_api);
	}


public:
	Engine(std::string address = "[::1]:4000") : susi_api{ new Susi::Api::ApiClient{address}} {
		susi_api->subscribe(std::string{"heartbeat::minute"},Susi::Events::Consumer{[this](Susi::Events::SharedEventPtr){
			auto event = susi_api->createEvent("session::update");
			susi_api->publish(std::move(event));
		}});
	}
	void populateController();

	std::shared_ptr<Susi::Api::ApiClient> getApi() {
		return susi_api;
	}

	void start() {
		for(auto & kv : controller){
			kv.second->start();
		}
	}

	void stop() {
		for(auto & kv : controller){
			kv.second->stop();
		}
	}

	~Engine() {
		stop();
	}
};

}
}

#endif // __CPPENGINE_ENGINE__
