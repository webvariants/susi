/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 *
 * @author: Thomas Krause (thomas.krause@webvariants.de)
 */

#ifndef __FORM_REQUEST_HANDLER__
#define __FORM_REQUEST_HANDLER__

#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Util/Application.h"

#include "Poco/Net/HTMLForm.h"
#include "logger/easylogging++.h"

#include "webstack/MyPartHandler.h"

namespace Susi {

    class FormRequestHandler: public Poco::Net::HTTPRequestHandler {
    private:
        std::string _uploadDirectory;
    public:
        FormRequestHandler( std::string uploadDirectory ) :
            _uploadDirectory( uploadDirectory ) {
            LOG(DEBUG) <<  "init form handler" ;
        }

        virtual void handleRequest( Poco::Net::HTTPServerRequest& request,
                                    Poco::Net::HTTPServerResponse& response ) override;
    };

}

#endif // __FORM_REQUEST_HANDLER__
