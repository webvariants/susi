/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 *
 * @author: Christian Sonderfeld (christian.sonderfeld@webvariants.de)
 */


#ifndef __CPPENGINE_ENGINE__
#define __CPPENGINE_ENGINE__

#include <memory>
#include <map>

#include "apiserver/ApiClient.h"
#include "BaseController.h"
#include "controller/SampleController.h"

namespace Susi {
namespace Cpp {

class Engine{
protected:
	static std::shared_ptr<Susi::Cpp::Engine> engineptr;

	std::shared_ptr<Susi::Api::ApiClient> susi_api;

	std::map<std::string, std::shared_ptr<Susi::Cpp::BaseController>> controller;

	void addController(std::string name, Susi::Cpp::BaseController *c) {
		controller[name] = std::shared_ptr<Susi::Cpp::BaseController>{c};
		controller[name]->setApi(susi_api);
	}

	Engine(std::string address = "[::1]:4000") : susi_api{ new Susi::Api::ApiClient{address}} {
	}

public:
	static std::shared_ptr<Susi::Cpp::Engine> getInstance() {
		if(!engineptr) {
			engineptr = std::shared_ptr<Engine>{new Engine()};
		}
		return engineptr;
	}

	void populateController();

	std::shared_ptr<Susi::Api::ApiClient> getApi() {
		return susi_api;
	}

	void start() {}

	void stop() {}

	~Engine() {
		stop();
	}
};

std::shared_ptr<Engine> Engine::engineptr{};

}
}

#endif // __CPPENGINE_ENGINE__
