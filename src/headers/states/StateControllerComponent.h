/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 * 
 * @author: Thomas Krause (thomas.krause@webvariants.de)
 */

#ifndef __STATE_CONTROLLER_COMPONENT__
#define __STATE_CONTROLLER_COMPONENT__

#include "world/BaseComponent.h"
#include "state/StateController.h"

namespace Susi {
	namespace States {
		class StateControllerComponent : Susi::System::BaseComponent ,  StateController {
		public:
			SessionManagerComponent (Susi::System::ComponentManager * mgr, std::string file) : 
				Susi::System::BaseComponent{mgr}, StateController{file} {}

			virtual void start() override {
				subscribe("syscall::startProcess", handleStartProcess);
			}

			virtual void stop() override {
				unsubscribeAll();
			}
		protected:
		};
	}
}

#endif // __STATE_CONTROLLER_COMPONENT__
