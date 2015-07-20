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

#ifndef __APIREQUESTHANDLER__
#define __APIREQUESTHANDLER__

#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Util/Application.h>
#include <Poco/DateTimeFormatter.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/File.h>
#include <Poco/FileStream.h>
#include <Poco/StreamCopier.h>

#include "susi/logger/easylogging++.h"
#include "susi/events/IEventSystem.h"

namespace Susi {

class ApiRequestHandler: public Poco::Net::HTTPRequestHandler {
  private:
    std::shared_ptr<Susi::Events::IEventSystem> _eventManager;
  public:
    ApiRequestHandler( std::shared_ptr<Susi::Events::IEventSystem> eventManager ) : _eventManager{eventManager} {
        LOG(DEBUG) <<  "init api handler" ;
    };
    virtual void handleRequest( Poco::Net::HTTPServerRequest& request,
                                Poco::Net::HTTPServerResponse& response ) override;
};

}

#endif // __APIREQUESTHANDLER__
