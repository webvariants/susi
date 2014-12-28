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

#ifndef __SUSISERVERCOMPONENTMANAGER__
#define __SUSISERVERCOMPONENTMANAGER__

#include "world/ComponentManager.h"
#include "util/Any.h"
 
#include "apiserver/TCPApiServerComponent.h"
#include "auth/AuthControllerComponent.h"
#include "db/DBComponent.h"
#include "enginestarter/EngineStarterComponent.h"
#include "events/EventManagerComponent.h"
#include "heartbeat/HeartBeatComponent.h"
#include "iocontroller/IOControllerComponent.h"
#include "sessions/SessionManagerComponent.h"
#include "states/StateControllerComponent.h"
#include "syscall/SyscallComponent.h"

//#include "webstack/HttpServerComponent.h"
#include "webstack/OnionHttpServerComponent.h"

#include "autodiscovery/AutoDiscoveryComponent.h"
#include "events/ConstraintController.h"
#include "ddhcp/DDHCPComponent.h"
#include "logger/Logger.h"
#include "duktape/DuktapeEngine.h"

namespace Susi {
namespace System {

class SusiServerComponentManager : public ComponentManager {
public:
	SusiServerComponentManager(Susi::Util::Any::Object config);
};

}
}

#endif // __SUSISERVERCOMPONENTMANAGER__
