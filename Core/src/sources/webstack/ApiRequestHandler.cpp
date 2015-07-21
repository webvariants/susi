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

#include "susi/webstack/ApiRequestHandler.h"

#include <mutex>
#include <condition_variable>
#include <atomic>

void Susi::ApiRequestHandler::handleRequest( Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response ) {
    Poco::Net::NameValueCollection cookies;
    request.getCookies( cookies );
    std::string id = cookies["susisession"];
    LOG(DEBUG) <<  "Api request from " + id + ": " + request.getURI();
    try {
        std::stringstream ss{};
        auto & body = request.stream();
        std::istreambuf_iterator<char> eos;
        std::string s{std::istreambuf_iterator<char>(body), eos};
        LOG(DEBUG) << "got "<<s<<" as api request";
        auto eventAny = Susi::Util::Any::fromJSONString(s);
        auto event = Susi::Events::Event{eventAny};
        auto evt = _eventManager->createEvent(event.topic);
        *evt = event;
        evt->sessionID = id;

        std::mutex mutex;
        std::condition_variable cond_var;
        std::atomic<bool> done;
        done.store(false);

        _eventManager->publish(std::move(evt),[this,&response,&done,&cond_var](Susi::Events::SharedEventPtr evt){
            LOG(DEBUG) << "finished publishing of "<<evt->toString();
            response.setChunkedTransferEncoding( true );
            response.setContentType( "application/json" );
            response.set("Access-Control-Allow-Origin", "*");
            response.set("Access-Control-Allow-Credentials", "*");
            std::ostream& ostr = response.send();
            ostr << evt->toString();
            done.store(true);
            cond_var.notify_one();
        });

        std::unique_lock<std::mutex> lk(mutex);
        cond_var.wait_for(lk,std::chrono::milliseconds{5000},[this,&done](){return done.load();});
        if(!done){
            throw std::runtime_error{"timeout"};
        }
    } catch ( const std::exception & e ) {
        LOG(DEBUG) <<  "error on api request: " << e.what();
        response.setChunkedTransferEncoding( true );
        response.setContentType( "text/html" );
        response.setStatus( Poco::Net::HTTPServerResponse::HTTPStatus::HTTP_NOT_FOUND );
        std::ostream& ostr = response.send();
        ostr << "<html><head><title>SUSI</title></head> ";
        ostr << "<body><p style=\"text-align: center;font-size: 48px;\"></br></br></br>";
        ostr << "(404) Not found: " + std::string( e.what() );
        ostr << "</p></body></html>";
    }
}
