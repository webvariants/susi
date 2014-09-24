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
#include "webstack/WebSocketRequestHandler.h"

#include "webstack/FormRequestHandler.h"

#include "apiserver/ApiServerComponent.h"

namespace Susi {

class RequestHandlerFactory: public Poco::Net::HTTPRequestHandlerFactory
{
protected:
    std::string _assetRoot;
    std::string _uploadDirectory;
    std::shared_ptr<Susi::Api::ApiServerComponent> _apiServer;
    std::shared_ptr<Susi::Sessions::SessionManagerComponent> _sessionManager;
public:
    RequestHandlerFactory(std::string assetRoot, 
                          std::string uploadDirectory,
                          std::shared_ptr<Susi::Api::ApiServerComponent> apiServer,                          
                          std::shared_ptr<Susi::Sessions::SessionManagerComponent> sessionManager) : 
                            _assetRoot{assetRoot}, 
                            _uploadDirectory{uploadDirectory},
                            _apiServer{apiServer},
                            _sessionManager{sessionManager} {}
    Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request){
        Susi::Logger::debug("got request with URI: "+request.getURI());
        try{
            if(request.getURI().find("/assets/")==0){
                Susi::Logger::debug("using assets handler");                
            	return new SessionRequestHandler(new AssetsRequestHandler(_assetRoot), _sessionManager);
            }else if(request.getURI() == "/ws"){
                Susi::Logger::debug("using websocket handler");
            	return new SessionRequestHandler(new WebSocketRequestHandler(_apiServer), _sessionManager);
            }/*else if(request.getURI() == "/compability"){
                return new SessionRequestHandler(new CompabilityRequestHandler());
            }*/else if(request.getURI() == "/form"){
                Susi::Logger::debug("using form handler");
                return new SessionRequestHandler(new FormRequestHandler(_uploadDirectory), _sessionManager);
            }else if(request.getURI() == "/"){
                Susi::Logger::debug("using redirect handler");
            	return new SessionRequestHandler(new RedirectRequestHandler(), _sessionManager);
            }
            Susi::Logger::debug("using not found handler");
            return new SessionRequestHandler(new NotFoundRequestHandler(), _sessionManager);
        }catch(const std::exception & e){
            Susi::Logger::error(std::string("error in request handler factory: ")+e.what());
            return nullptr;
        }
    }
};

}

#endif // __REQUESTHANDLERFACTORY__