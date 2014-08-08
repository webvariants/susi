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

#ifndef __TCPSERVER__
#define __TCPSERVER__

#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include <arpa/inet.h>

#include <cstring>
#include <iostream>
#include <chrono>
#include <map>
#include <stdexcept>
#include <thread>

namespace Susi {
namespace Api {

class TCPServer {
protected:
	struct Context{
		TCPServer *server;
		std::string sessionID;
	};
    struct event_base *base;
    std::map<std::string,struct evbuffer *> outputs;
    std::map<std::string,evutil_socket_t> sockets;
    std::thread runloop;
	

public:

	virtual void onConnect(std::string & sessionID) {}
	virtual void onData(std::string & sessionID, std::string & data) = 0;
	virtual void onClose(std::string & sessionID) {};

	TCPServer(unsigned short port);

	void send(std::string & sessionID, std::string & data);
	void close(std::string & sessionID);
	void join();
	void stop();

	virtual ~TCPServer();
};

}
}

#endif // __TCPSERVER__