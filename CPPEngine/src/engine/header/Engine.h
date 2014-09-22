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

#include "apiserver/ApiClient.h"

namespace Susi {
namespace Cpp {

class Engine : public Susi::Api::ApiClient {
protected:
	static std::shared_ptr<Engine> engineptr;

	std::map<std::string, Susi::Cpp::BaseController> controller;

	Engine(std::string address = "[::1]:4000") : ApiClient{address} {
	}

public:
	static std::shared_ptr<Engine> getInstance() {
		if(!engineptr) {
			engineptr = std::shared_ptr<Engine>{new Engine()};
		}
		return engineptr;
	}

	virtual void populateController();

	void start();

	void stop();

	~Engine() {
		stop();
	}
};

}
}

#endif // __CPPENGINE_ENGINE__
