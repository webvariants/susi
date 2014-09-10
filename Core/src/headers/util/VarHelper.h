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

#ifndef __VARHELPER__
#define __VARHELPER__

#include <Poco/Dynamic/Var.h>
#include <Poco/JSON/Object.h>

namespace Susi { namespace Util {

Poco::Dynamic::Var getFromVar(Poco::Dynamic::Var & var, std::string key);

}}

#endif // __VARHELPER__