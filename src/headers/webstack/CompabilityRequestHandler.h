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

#ifndef __COMPABILITYREQUESTHANDLER__
#define __COMPABILITYREQUESTHANDLER__

#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Util/Application.h"
#include "Poco/JSON/Parser.h"

#include "events/EventSystem.h"

namespace Susi {

class CompabilityRequestHandler: public Poco::Net::HTTPRequestHandler {
public:
	CompabilityRequestHandler() {}
    void handleRequest(Poco::Net::HTTPServerRequest& request,
                       Poco::Net::HTTPServerResponse& response);
};

}

#endif // __COMPABILITYREQUESTHANDLER__
