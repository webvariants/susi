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

#ifndef __SYS_CALL_CONTROLLER_COMPONENT__
#define __SYS_CALL_CONTROLLER_COMPONENT__

#include "world/BaseComponent.h"
#include "syscall/SysCallController.h"

namespace Susi {
	namespace Syscall {
		class SyscallControllerComponent : Susi::System::BaseComponent ,  Controller {
		public:
			SessionManagerComponent (Susi::System::ComponentManager * mgr, std::string config_path) : 
				Susi::System::BaseComponent{mgr}, Controller{config_path} {}

			virtual void start() override {
				subscribe("syscall::startProcess", handleStartProcess);
			}

			virtual void stop() override {
				unsubscribeAll();
			}
		protected:
			void handleStartProcess(Susi::Events::EventPtr event) {
				try{		
					std::string process_type = event->payload["process_type"];
					std::map<std::string, std::string> argsReplace = event->payload["argsReplace"];

					event->payload["success"] = startProcess(process_type, argsReplace);				
				}catch(const std::exception & e){
					std::string msg = "Error in handleStartProcess(): ";
					msg += e.what();
					throw std::runtime_error(msg);
				}
			}
		};
	}
}

#endif // __SYS_CALL_CONTROLLER_COMPONENT__
