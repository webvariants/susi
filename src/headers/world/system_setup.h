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

#ifndef __SYSTEMSETUP__
#define __SYSTEMSETUP__

#include "world/ComponentManager.h"
#include "util/Any.h"

namespace Susi {
namespace System {

std::shared_ptr<Susi::System::ComponentManager> createSusiComponentManager(Susi::Util::Any::Object config);

}
}

#endif // __SYSTEMSETUP__

