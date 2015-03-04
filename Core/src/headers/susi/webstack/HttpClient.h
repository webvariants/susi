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

#ifndef __HTTP_CLIENT__
#define __HTTP_CLIENT__

#include <iostream>
#include <sstream>
#include <string>
#include <memory>

#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/WebSocket.h>
#include <Poco/StreamCopier.h>

#include <Poco/URI.h>

#include <Poco/Path.h>

#include <Poco/Exception.h>

namespace Susi {
    class HttpClient {
    protected:
        Poco::URI _uri;
        Poco::Net::HTTPClientSession _session;
        //Poco::Net::NameValueCollection _cookies;
        Poco::Net::HTTPResponse::HTTPStatus _status;
        std::shared_ptr<Poco::Net::WebSocket> _websocket;
        std::vector<std::pair<std::string,std::string>> _headers;

        Poco::Net::HTTPRequest ws_req {Poco::Net::HTTPRequest::HTTP_GET, "/ws", Poco::Net::HTTPMessage::HTTP_1_1};
        Poco::Net::HTTPResponse ws_resp;

        void parseResponse( Poco::Net::HTTPResponse & resp ) {
            _status = resp.getStatus();
            /*std::vector<Poco::Net::HTTPCookie> cookies;
            resp.getCookies(cookies);
            for(auto & cookie : cookies){
                _cookies.add(cookie.getName(),cookie.getValue());
            }*/
            _headers.clear();
            for( auto & kv : resp ) {
                _headers.push_back( kv );
            }
        }

    public:
        HttpClient( std::string uri ) : _uri {uri}, _session {_uri.getHost(),_uri.getPort()} {}

        /*void addCookie(std::string key, std::string value){
            _cookies.add(key,value);
        }*/
        void addHeader( std::string key, std::string value ) {
            _headers.push_back( std::pair<std::string,std::string> {key,value} );
        }

        std::vector<std::pair<std::string,std::string>> getHeaders() {
            return _headers;
        }

        Poco::Net::HTTPResponse::HTTPStatus getStatus() {
            return _status;
        }

        std::string get( std::string path ) {
            Poco::Net::HTTPRequest req( Poco::Net::HTTPRequest::HTTP_GET, path, Poco::Net::HTTPMessage::HTTP_1_1 );
            //req.setCookies(_cookies);
            for( auto & kv : _headers ) {
                req.add( kv.first,kv.second );
            }
            _session.sendRequest( req );
            Poco::Net::HTTPResponse resp;
            std::istream & dataStream = _session.receiveResponse( resp );
            std::istreambuf_iterator<char> eos;
            std::string body( std::istreambuf_iterator<char>( dataStream ), eos );
            parseResponse( resp );
            return body;
        }

        std::string post( std::string path, std::string data ) {
            Poco::Net::HTTPRequest req( Poco::Net::HTTPRequest::HTTP_POST, path, Poco::Net::HTTPMessage::HTTP_1_1 );
            //req.setCookies(_cookies);
            for( auto & kv : _headers ) {
                req.add( kv.first,kv.second );
            }
            std::ostream & toServerStream = _session.sendRequest( req );
            toServerStream << data;
            Poco::Net::HTTPResponse resp;
            std::istream & fromServerStream = _session.receiveResponse( resp );
            std::istreambuf_iterator<char> eos;
            std::string body( std::istreambuf_iterator<char>( fromServerStream ), eos );
            parseResponse( resp );
            return body;
        }

        void connectWebSocket() {
            _websocket = std::make_shared<Poco::Net::WebSocket>( _session,ws_req,ws_resp );
            parseResponse( ws_resp );
        }
        void send( std::string data ) {
            _websocket->sendFrame( data.c_str(),data.size() );
        }
        std::string recv() {
            char buff[4096];
            int flags;
            size_t bs = _websocket->receiveFrame( buff,4096,flags );
            return std::string {buff,bs};
        }
    };
}

#endif // __HTTP_CLIENT__