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

#ifndef __TINYJSENGINE__
#define __TINYJSENGINE__

#include "tiny-js/TinyJS.h"
#include <memory>
#include <fstream>
#include <iostream>

namespace Susi {

class TinyJSEngine {
private:
	std::shared_ptr<CTinyJS> js;
public:
	TinyJSEngine();
	TinyJSEngine(std::string source);
	std::string run(std::string source);
	std::string runFile(std::string filename);
};

}

#endif // __TINYJSENGINE__