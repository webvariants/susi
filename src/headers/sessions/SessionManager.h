/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 *
 * @author: Tino Rusch (tino.rusch@webvariants.de)
 */

#ifndef __SESSIONMANAGER__
#define __SESSIONMANAGER__

#include <map>
#include <Poco/Timestamp.h>
#include <mutex>
#include <chrono>
#include <thread>
#include <Poco/Util/Application.h>
#include <Poco/Dynamic/Var.h>
#include "sessions/Session.h"
#include "events/EventSystem.h"

namespace Susi{
	namespace Sessions {
		class SessionManager {
		protected:
			std::map<std::string, Session> sessions;
			std::mutex mutex;
			std::chrono::milliseconds stdLifetime;
			bool initialized = false;
			long subId;
			int checkSessions();
		public:
			bool checkSession(std::string id);
			void updateSession(std::string id);
			void updateSession(std::string id, std::chrono::milliseconds lifeTime);
			bool killSession(std::string id);
			bool setSessionAttribute(std::string sessionID, std::string key, Poco::Dynamic::Var value);
			Poco::Dynamic::Var getSessionAttribute(std::string sessionID, std::string key);
			bool init(std::chrono::milliseconds stdSessionLifetime, std::chrono::milliseconds checkInterval);
			bool pushSessionAttribute(std::string sessionID, std::string key, Poco::Dynamic::Var value);
			bool removeSessionAttribute(std::string sessionID, std::string key);

			~SessionManager();
		};
	}
}

#endif //__SESSIONMANAGER__
