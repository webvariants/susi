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

#ifndef __UTIL_HELPERS__
#define __UTIL_HELPERS__

#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>

namespace Susi {
	namespace Util {
		class Helpers {
			public:
				static std::vector<std::string> & split(const std::string &s, char delim, std::vector<std::string> &elems);
				static bool isInteger(const std::string & s);
				static bool isDouble(const std::string & s);
		};
	}
}

#endif // __UTIL_HELPERS__
