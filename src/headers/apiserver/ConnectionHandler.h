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

#ifndef __CONNECTIONHANDLER__
#define __CONNECTIONHANDLER__

#include "apiserver/Connection.h"
#include "events/EventSystem.h"
#include "util/VarHelper.h"
#include "util/Any.h"
#include <Poco/Dynamic/Struct.h>
#include <Poco/Dynamic/Var.h>
#include <memory>
#include <map>

namespace Susi {

class ConnectionHandler {
public:
	static void handleConnection(std::shared_ptr<Susi::Connection> conn);
protected:
	static void handlePacket(std::shared_ptr<Susi::Connection> conn, Poco::Dynamic::Var packet);
	static void handlePublish(std::shared_ptr<Susi::Connection> conn, Poco::Dynamic::Var packet);
	static void handleSubscribe(std::shared_ptr<Susi::Connection> conn, Poco::Dynamic::Var packet);	
	static void handleUnsubscribe(std::shared_ptr<Susi::Connection> conn, Poco::Dynamic::Var packet);	
	static void sendStatus(std::shared_ptr<Susi::Connection> conn, Poco::Dynamic::Var & request, std::string msg,bool success = true);
};	
	
}


#endif // __CONNECTIONHANDLER__