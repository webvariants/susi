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

#ifndef __SESSION_MANAGER_COMPONENT__
#define __SESSION_MANAGER_COMPONENT__

#include "world/BaseComponent.h"
#include "sessions/SessionManager.h"

namespace Susi {
	namespace Sessions {
		class SessionManagerComponent : Susi::System::BaseComponent ,  SessionManager {
		public:
			SessionManagerComponent (Susi::System::ComponentManager * mgr) : 
				Susi::System::BaseComponent{mgr}, SessionManager{} {}

			virtual void start() override {
				subscribe("session::setAttribute", handleSetAttribute);
				subscribe("session::getAttribute", handleGetAttribute);
				subscribe("session::pushAttribute", handlePushAttribute);
				subscribe("session::removeAttribute", handleRemoveAttribute);
				subscribe("session::update", handleUpdate);
				subscribe("session::check", handleCheck);
			}

			virtual void stop() override {
				unsubscribeAll();
			}
		protected:
			void handleGetAttribute(Susi::Events::EventPtr event) {
				try{
					std::string sessionID = event->payload["id"];
					std::string key = event->payload["key"];

					event->payload["value"] = getSessionAttribute(sessionID, key);		
				}catch(const std::exception & e){
					std::string msg = "Error in handleSessionGetAttribute(): ";
					msg += e.what();
					throw std::runtime_error(msg);		
				}
			}
			void handleSetAttribute(Susi::Events::EventPtr event) {
				try{
					std::string sessionID = event->payload["id"];
					std::string key = event->payload["key"];
					Susi::Util::Any value = event->payload["value"];

					event->payload["success"] = setSessionAttribute(sessionID, key, value);
				}catch(const std::exception & e){
					std::string msg = "Error in handleSessionSetAttribute(): ";
					msg += e.what();
					throw std::runtime_error(msg);
				}
			}
			void handlePushAttribute(Susi::Events::EventPtr event) {
				try{
					std::string sessionID = event->payload["id"];
					std::string key = event->payload["key"];
					Susi::Util::Any value = event->payload["value"];

					event->payload["success"] = pushSessionAttribute(sessionID, key, value);
				}catch(const std::exception & e){
					std::string msg = "Error in handleSessionPushAttribute(): ";
					msg += e.what();
					throw std::runtime_error(msg);
				}
			}
			void handleRemoveAttribute(Susi::Events::EventPtr event) {
				try{
					std::string sessionID = event->payload["id"];
					std::string key = event->payload["key"];

					event->payload["success"] = removeSessionAttribute(sessionID, key);
				}catch(const std::exception & e){
					std::string msg = "Error in handleSessionRemoveAttribute(): ";
					msg += e.what();
					throw std::runtime_error(msg);
				}
			}
			void handleUpdate(Susi::Events::EventPtr event) {
				try{
					std::string sessionID = event->payload["id"];
					std::chrono::seconds secs;

					int seconds = event->payload["seconds"];

					if(seconds > 0) {
						secs = std::chrono::seconds(seconds);
						updateSession(sessionID, std::chrono::duration_cast<std::chrono::milliseconds>(secs));
					}
					else {
						updateSession(sessionID);
					}

					event->payload["success"] = true;
				}catch(const std::exception & e){
					std::string msg = "Error in handleUpdateSession(): ";
					msg += e.what();
					throw std::runtime_error(msg);
				}
			}
			void handleCheck(Susi::Events::EventPtr event) {
				try{
					std::string sessionID = event->payload["id"];
					event->payload["success"] = checkSession(sessionID);		
				}catch(const std::exception & e){
					std::string msg = "Error in handleCheckSession(): ";
					msg += e.what();
					throw std::runtime_error(msg);
				}
			}
		};
	}
}

#endif // __SESSION_MANAGER_COMPONENT__
