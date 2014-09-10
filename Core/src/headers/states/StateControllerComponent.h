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
#include "states/StateController.h"

namespace Susi {
	namespace States {
		class StateControllerComponent : public Susi::System::BaseComponent , public StateController {
		public:
			StateControllerComponent (Susi::System::ComponentManager * mgr, std::string file) :
				Susi::System::BaseComponent{mgr}, StateController{file} {}

			virtual void start() override {
				subscribe("heartbeat::fiveMinute",[this](::Susi::Events::EventPtr evt){handleSave(std::move(evt));});

				subscribe("state::setState", [this](::Susi::Events::EventPtr evt){handleSetState(std::move(evt));});
				subscribe("state::getState", [this](::Susi::Events::EventPtr evt){handleGetState(std::move(evt));});
				subscribe("state::setPersistentState", [this](::Susi::Events::EventPtr evt){handleSetPersistentState(std::move(evt));});
				subscribe("state::getPersistentState", [this](::Susi::Events::EventPtr evt){handleGetPersistentState(std::move(evt));});
			}

			virtual void stop() override {
				unsubscribeAll();
			}
		protected:
			void handleSave(Susi::Events::EventPtr event) {
				if(persistentChanged) {
					savePersistent();
					persistentChanged = false;
				}
			}

			void handleSetState(Susi::Events::EventPtr event) {
				std::cout<<"got state event"<<std::endl;
				try{
					std::string stateID  = event->payload["stateID"];
					Susi::Util::Any value = event->payload["value"];
					event->payload["success"] = setState(stateID, value);
				}catch(const std::exception & e){
					std::string msg = "Error in handleSetState(): ";
					msg += e.what();
					throw std::runtime_error(msg);
				}
			}

			void handleGetState(Susi::Events::EventPtr event) {
				try{
					std::string stateID  = event->payload["stateID"];
					event->payload["value"] = getState(stateID);
				}catch(const std::exception & e){
					std::string msg = "Error in handleGetState(): ";
					msg += e.what();
					throw std::runtime_error(msg);
				}
			}

			void handleSetPersistentState(Susi::Events::EventPtr event) {
				std::cout<<"got persitent state event"<<std::endl;
				try{
					std::string stateID  = event->payload["stateID"];
					Susi::Util::Any value = event->payload["value"];
					event->payload["success"] = setPersistentState(stateID, value);
				}catch(const std::exception & e){
					std::string msg = "Error in handleSetPersistentState(): ";
					msg += e.what();
					throw std::runtime_error(msg);
				}
			}

			void handleGetPersistentState(Susi::Events::EventPtr event) {
				try{
					std::string stateID  = event->payload["stateID"];
					event->payload["value"] = getPersistentState(stateID);
				}catch(const std::exception & e){
					std::string msg = "Error in handleGetPersistentState(): ";
					msg += e.what();
					throw std::runtime_error(msg);
				}
			}
		};
	}
}

#endif // __STATE_CONTROLLER_COMPONENT__
