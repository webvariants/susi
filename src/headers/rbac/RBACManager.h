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

#ifndef __RBACMANAGER__
#define __RBACMANAGER__

#include "events/EventSystem.h"
#include "iocontroller/IOController.h"
#include "logger/Logger.h"

#include <map>
#include <vector>
#include <sstream>

#include <Poco/Dynamic/Var.h>
#include <Poco/JSON/Parser.h>

namespace Susi {
	class RBACManager {
	protected:
		bool defaultRead = true;
		bool defaultWrite = false;
		struct Entry {
			std::string role;
			bool read,write;
		};
		std::map<std::string,std::vector<Entry>> _acl;
	public:

		RBACManager();
		RBACManager(std::string fileLocation);	

		void setRead(std::string object, std::string role, bool read);
		bool getRead(std::string object, std::string role);

		void setWrite(std::string object, std::string role, bool write);
		bool getWrite(std::string object, std::string role);
		
		void setDefaultRead(bool read);
		void setDefaultWrite(bool write);

		bool loadFile(std::string fileLocation);
		bool saveFile(std::string fileLocation);
	};
}

#endif // __RBACMANAGER__