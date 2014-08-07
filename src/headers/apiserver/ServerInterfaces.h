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

#ifndef __SERVERINTERFACES__
#define __SERVERINTERFACES__

#include "util/Any.h"
#include <string>

namespace Susi {
namespace Api {


class DataServer {
public:
	virtual void onConnect(std::string & id){};
	virtual void onData(std::string & id, std::string & data) = 0;
	virtual void onClose(std::string & id){};

	virtual void send(std::string & id, std::string & data) = 0;
	virtual void close(std::string & id) = 0;
	virtual void run() = 0;
	virtual void join() = 0;
	virtual void stop() = 0;
};

class MessageServer {
public:
	virtual void onConnect(std::string & id){};
	virtual void onMessage(std::string & id, Susi::Util::Any & data) = 0;
	virtual void onClose(std::string & id){};

	virtual void send(std::string & id, Susi::Util::Any & data) = 0;
	virtual void close(std::string & id) = 0;
	virtual void run() = 0;
	virtual void join() = 0;
	virtual void stop() = 0;
};

}
}

#endif // __SERVERINTERFACES__