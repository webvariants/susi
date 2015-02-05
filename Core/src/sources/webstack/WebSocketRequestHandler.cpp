/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 *
 * @author: Tino Rusch (tino.rusch@webvariants.de), Thomas Krause (thomas.krause@webvariants.de)
 */

#include "webstack/WebSocketRequestHandler.h"

Susi::WebSocketRequestHandler::WebSocketRequestHandler( std::shared_ptr<Susi::Api::ApiServerComponent> apiServer, std::shared_ptr<Susi::Sessions::SessionManagerComponent> sessionManager) :
    _apiServer{apiServer},
    _sessionManager{sessionManager} {}

void Susi::WebSocketRequestHandler::handleRequest( Poco::Net::HTTPServerRequest& request,
        Poco::Net::HTTPServerResponse& response ) {
    Poco::Net::WebSocket socket( request,response );
    socket.setSendTimeout(200000);
    socket.setReceiveTimeout(200000);
    Poco::Net::NameValueCollection cookies;
    request.getCookies( cookies );
    std::string id = cookies["susisession"];
    Poco::Timestamp now;
    
    std::string connId = std::to_string( now.epochMicroseconds() );
    _sessionManager->addAlias(connId,id);

    LOG(DEBUG) <<  "register sender in ws-handler for session "<<id<<" with connection id "<<connId;
    _apiServer->registerSender( connId,[&socket]( Susi::Util::Any & arg ) {
        std::string msg = arg.toJSONString();
        //LOG(DEBUG) <<  "send frame to websocket" ;
        try{
            socket.sendFrame( msg.data(), msg.length(), Poco::Net::WebSocket::FRAME_TEXT );
        }catch(...){
            LOG(DEBUG) << "send timeout in websocket sender!";
        }
    } );

    _apiServer->onConnect( connId );

    char buffer[4096];
    int flags;
    size_t n;

    while( true ) {
        try{
            n = socket.receiveFrame( buffer, sizeof( buffer ), flags );
            LOG(DEBUG) << "got websocket data";
            if( n==0 || ( flags & Poco::Net::WebSocket::FRAME_OP_BITMASK ) == Poco::Net::WebSocket::FRAME_OP_CLOSE ) {
                LOG(DEBUG) << "closing websocket connection "<<connId<<" (session: "<<id<<") normally";
                break;
            }
            std::string str( buffer, n );
            Susi::Util::Any packet = Susi::Util::Any::fromJSONString( str );
            _apiServer->onMessage( connId, packet );
        } catch(const Poco::TimeoutException &){
            LOG(DEBUG) << "got websocket timeout";
        } catch(const std::exception & e){
            LOG(DEBUG) << "closing websocket connection "<<connId<<" (session: "<<id<<") because of exception: "<<e.what();
            break;
        } catch(...){
            LOG(DEBUG) << "closing websocket connection "<<connId<<" (session: "<<id<<") because of unknown exception";
            break;
        }
    }
    _apiServer->onClose( connId );
}
