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
		class SessionManagerComponent : public Susi::System::BaseComponent , public SessionManager {
		public:
			SessionManagerComponent (Susi::System::ComponentManager * mgr, std::chrono::milliseconds stdSessionLifetime) :
				Susi::System::BaseComponent{mgr}, SessionManager{} {
					_stdSessionLifetime = stdSessionLifetime;					
				}

			~SessionManagerComponent() {
				stop();
			}

			virtual void start() override {
				init(_stdSessionLifetime);

				// Consumer
				Susi::Events::Consumer handler = [this](::Susi::Events::SharedEventPtr evt){handleCheckSessions(std::move(evt));};
				subscribe("heartbeat::one", handler);

				// Processor
				subscribe("session::setAttribute", [this](::Susi::Events::EventPtr evt){handleSetAttribute(std::move(evt));});
				subscribe("session::getAttribute", [this](::Susi::Events::EventPtr evt){handleGetAttribute(std::move(evt));});
				subscribe("session::pushAttribute", [this](::Susi::Events::EventPtr evt){handlePushAttribute(std::move(evt));});
				subscribe("session::removeAttribute", [this](::Susi::Events::EventPtr evt){handleRemoveAttribute(std::move(evt));});
				subscribe("session::update", [this](::Susi::Events::EventPtr evt){handleUpdate(std::move(evt));});
				subscribe("session::check", [this](::Susi::Events::EventPtr evt){handleCheck(std::move(evt));});
			}

			virtual void stop() override {
				unsubscribeAll();
			}

		protected:
			std::chrono::milliseconds _stdSessionLifetime;			

			void handleCheckSessions(Susi::Events::SharedEventPtr event) {
				checkSessions();
			}

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
