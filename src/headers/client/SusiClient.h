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

#ifndef __SUSICLIENT__
#define __SUSICLIENT__

#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/SocketStream.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Dynamic/Var.h>

#include <thread>

#include "events/EventSystem.h"
#include "apiserver/Connection.h"
#include "util/VarHelper.h"

namespace Susi {

class SusiClient {
private:
	Poco::Net::SocketAddress addr;
	Poco::Net::StreamSocket sock;
	Susi::TCPConnection conn;
	std::map<std::string,std::function<void(Event&)>> subscriptions;
public:
	SusiClient(std::string address) : addr(address), sock(addr), conn(sock) {}
	void publish(std::string topic, Poco::Dynamic::Var payload,std::string returnAddr);
	bool subscribe(std::string topic, std::function<void(Event&)> callback);
	bool unsubscribe(std::string topic);
	void getEvent();
	void close(){
		sock.close();
	}
};

}

#endif // __SUSICLIENT__