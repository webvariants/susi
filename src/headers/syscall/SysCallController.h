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

#ifndef __SYS_CALL_CONTROLLER__
#define __SYS_CALL_CONTROLLER__

#include <Poco/Dynamic/Var.h>
#include <Poco/Thread.h>
#include <Poco/ThreadPool.h>
#include <Poco/Path.h>
#include <Poco/Environment.h>

#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Array.h>

#include <iostream>
#include <map>
#include <tuple>
#include <memory>

#include "syscall/SysCallWorker.h"
#include "syscall/SysCallProcessItem.h"
#include "iocontroller/IOController.h"


namespace Susi {
	namespace Syscall {
		class Controller{
				std::map <std::string, Susi::Syscall::ProcessItem> processMap;
			public:
				Poco::ThreadPool pool; // by default holds max 16 threads

				Controller();
				Controller(std::string config_path);

				bool startProcess(std::string returnAddr, std::string process_type, std::map<std::string, std::string> argsReplace);
		};
	}
}

#endif // __SYS_CALL_CONTROLLER__