/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 *
 * @author: Christian Sonderfeld (christian.sonderfeld@webvariants.de)
 */


#ifndef __SYSTEMSETUP__
#define __SYSTEMSETUP__

#include "util/Any.h"
#include "world/ComponentManager.h"

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
#include "webstack/HttpServerComponent.h"
#include "autodiscovery/AutoDiscoveryComponent.h"
#include "events/ConstraintController.h"

namespace Susi {
    namespace System {

        std::shared_ptr<Susi::System::ComponentManager> createSusiComponentManager( Susi::Util::Any::Object config );

    }
}

#endif // __SYSTEMSETUP__
