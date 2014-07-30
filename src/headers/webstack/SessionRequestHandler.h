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

#ifndef __SESSIONREQUESTHANDLER__
#define __SESSIONREQUESTHANDLER__

#include <iostream>

#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Util/Application.h"

#include "sessions/SessionManager.h"

namespace Susi {


class SessionRequestHandler: public Poco::Net::HTTPRequestHandler {
protected:
	Poco::Net::HTTPRequestHandler *defaultHandler;
public:
    SessionRequestHandler(Poco::Net::HTTPRequestHandler *defaultHandler) : defaultHandler{defaultHandler} {}
    ~SessionRequestHandler(){delete defaultHandler;}
    void handleRequest(Poco::Net::HTTPServerRequest& request,Poco::Net::HTTPServerResponse& response);
};

}

#endif // __SESSIONREQUESTHANDLER__
