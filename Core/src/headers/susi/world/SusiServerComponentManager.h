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

#include "susi/world/PluginLoadingComponentManager.h"
#include "susi/util/Any.h"
#include "susi/apiserver/TCPApiServerComponent.h"
#include "susi/auth/AuthControllerComponent.h"
#include "susi/db/DBComponent.h"
#include "susi/enginestarter/EngineStarterComponent.h"
#include "susi/events/EventManagerComponent.h"
#include "susi/heartbeat/HeartBeatComponent.h"
#include "susi/iocontroller/IOControllerComponent.h"
#include "susi/sessions/SessionManagerComponent.h"
#include "susi/states/StateControllerComponent.h"
#include "susi/syscall/SyscallComponent.h"
#include "susi/webstack/HttpServerComponent.h"
#include "susi/autodiscovery/AutoDiscoveryComponent.h"
#include "susi/events/ConstraintController.h"
#include "susi/ddhcp/DDHCPComponent.h"
#include "susi/logger/Logger.h"
#include "susi/duktape/DuktapeEngine.h"
#include "susi/selfchecker/SelfChecker.h"
#include "susi/coapserver/CoapServerComponent.h"

namespace Susi {
namespace System {

class SusiServerComponentManager : public PluginLoadingComponentManager {
public:
	SusiServerComponentManager(Susi::Util::Any::Object config);
};

}
}

#endif // __SUSISERVERCOMPONENTMANAGER__
