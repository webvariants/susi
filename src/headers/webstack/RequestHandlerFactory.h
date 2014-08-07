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

#ifndef __REQUESTHANDLERFACTORY__
#define __REQUESTHANDLERFACTORY__

#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Util/Application.h"
#include "webstack/NotFoundRequestHandler.h"
#include "webstack/AssetsRequestHandler.h"
#include "webstack/SessionRequestHandler.h"
#include "webstack/RedirectRequestHandler.h"
//#include "webstack/WebSocketRequestHandler.h"
#include "webstack/CompabilityRequestHandler.h"

namespace Susi {

class RequestHandlerFactory: public Poco::Net::HTTPRequestHandlerFactory
{
protected:
    std::string _assetRoot;
public:
    RequestHandlerFactory(std::string assetRoot) : _assetRoot{assetRoot} {}
    Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request){
        Susi::debug("got request with URI: "+request.getURI());
        try{
            if(request.getURI().find("/assets/")==0){
            	return new SessionRequestHandler(new AssetsRequestHandler(_assetRoot));
            }/*else if(request.getURI() == "/ws"){
            	return new SessionRequestHandler(new WebSocketRequestHandler());
            }*/else if(request.getURI() == "/compability"){
                return new SessionRequestHandler(new CompabilityRequestHandler());
            }else if(request.getURI() == "/"){
            	return new SessionRequestHandler(new RedirectRequestHandler());
            }
            return new SessionRequestHandler(new NotFoundRequestHandler());
        }catch(const std::exception & e){
            Susi::error(std::string("error in request handler factory: ")+e.what());
            return nullptr;
        }
    }
};

}

#endif // __REQUESTHANDLERFACTORY__