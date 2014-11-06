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

#ifndef __ASSETSREQUESTHANDLER__
#define __ASSETSREQUESTHANDLER__

#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Util/Application.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/File.h"
#include "Poco/FileStream.h"
#include "Poco/StreamCopier.h"

#include "logger/easylogging++.h"

namespace Susi {

    class AssetsRequestHandler: public Poco::Net::HTTPRequestHandler {
    private:
        Poco::File _rootDirectory;
    public:
        AssetsRequestHandler( std::string rootDirectory ) :
            _rootDirectory( rootDirectory ) {
            LOG(DEBUG) <<  "init assets handler" ;
        }
        virtual void handleRequest( Poco::Net::HTTPServerRequest& request,
                                    Poco::Net::HTTPServerResponse& response ) override;
    };

}

#endif // __ASSETSREQUESTHANDLER__
