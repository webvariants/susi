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

#ifndef __NOTFOUNDREQUESTHANDLER__
#define __NOTFOUNDREQUESTHANDLER__

#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Util/Application.h"

namespace Susi {

    class NotFoundRequestHandler: public Poco::Net::HTTPRequestHandler {
    public:
        void handleRequest( Poco::Net::HTTPServerRequest& request,
                            Poco::Net::HTTPServerResponse& response );
    };

}

#endif // __NOTFOUNDREQUESTHANDLER__
