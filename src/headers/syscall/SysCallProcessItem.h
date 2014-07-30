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

#ifndef __SYS_CALL_PROCESS_ITEM__
#define __SYS_CALL_PROCESS_ITEM__

#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>

namespace Susi {
	namespace Syscall {
		class ProcessItem {
			public:
				std::string cmd;
				std::string args;
				bool background;

				ProcessItem();
				ProcessItem(std::string cmd, std::string args, bool background);

				std::vector<std::string> & split(const std::string &s, char delim, std::vector<std::string> &elems);
				std::vector<std::string> getArgs(std::map<std::string, std::string> argsReplace);
		};
	}
}

#endif // __SYS_CALL_PROCESS_ITEM__