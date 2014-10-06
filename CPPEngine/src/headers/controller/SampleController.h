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


#ifndef __SUSI_CPPENGINE_SAMPLECONTROLLER__
#define __SUSI_CPPENGINE_SAMPLECONTROLLER__

#include "engine/BaseController.h"

namespace Susi {
namespace Cpp {

class SampleController : public Susi::Cpp::BaseController {
public:
	virtual void start() override {
		Susi::Events::Consumer c = [this](Susi::Events::SharedEventPtr event){
			sampleConsumerFunction(event);
		};
		Susi::Events::Processor p = [this](Susi::Events::EventPtr event){
			sampleProcessorFunction(std::move(event));
		};
		subscribe("samplecontroller::test1", c , 3);
		subscribe("samplecontroller::test2", p , 3);
	}

	void sampleConsumerFunction(Susi::Events::SharedEventPtr event){
		log("Funktioniert!");
	}

	void sampleProcessorFunction(Susi::Events::EventPtr event){
		event->payload = "processed!";
	}
};

}
}

#endif // __SUSI_CPPENGINE_SAMPLECONTROLLER__
