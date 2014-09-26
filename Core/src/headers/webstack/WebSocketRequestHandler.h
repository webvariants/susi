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

#ifndef __WEBSOCKETREQUESTHANDLER__
#define __WEBSOCKETREQUESTHANDLER__

#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPRequestHandler.h"

#include "Poco/Net/WebSocket.h"

#include <chrono>
#include "apiserver/ApiServerComponent.h"
#include "util/Any.h"
#include "logger/Logger.h"

namespace Susi {

    class WebSocketRequestHandler: public Poco::Net::HTTPRequestHandler {
    protected:
        std::shared_ptr<Susi::Api::ApiServerComponent> _apiServer;
    public:
        void handleRequest( Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response );
        WebSocketRequestHandler( std::shared_ptr<Susi::Api::ApiServerComponent> apiServer );
    };

}

#endif // __WEBSOCKETREQUESTHANDLER__
