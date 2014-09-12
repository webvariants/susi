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

#ifndef __TINYJS_ENGINE__
#define __TINYJS_ENGINE__

#include "tiny-js-read-only/TinyJS.h"

#include <iostream>
#include "logger/Logger.h"
#include "util/Any.h"

namespace Susi {
	namespace JS {
		namespace Tiny {
			class Engine {
				public:
					Engine(std::string addr, std::string file="") {}

			};
		}
	}	
}

#endif // __TINYJS_ENGINE__
