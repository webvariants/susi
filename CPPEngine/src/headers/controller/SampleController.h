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


#ifndef __SUSI_CPPENGINE_SAMPLECONTROLLER__
#define __SUSI_CPPENGINE_SAMPLECONTROLLER__

#include "engine/BaseController.h"

namespace Susi {
namespace Cpp {

class SampleController : public Susi::Cpp::BaseController {
public:
	virtual void start() override {
		std::cout<<"starting sample!"<<std::endl;
		Susi::Events::Consumer c = [this](Susi::Events::SharedEventPtr event) {
			log("Funktioniert!");
		};
		subscribe("samplecontroller::test", c , 3);
	}
};

}
}

#endif // __SUSI_CPPENGINE_SAMPLECONTROLLER__
