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

Susi::WebSocketRequestHandler::WebSocketRequestHandler(Susi::Api::ApiServer* server) {
	apiServer = server;
}

void Susi::WebSocketRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request,
                       							Poco::Net::HTTPServerResponse& response) {
	Poco::Net::WebSocket socket(request,response);
	Poco::Net::NameValueCollection cookies;
	request.getCookies(cookies);
	std::string id = cookies["susisession"];
    
    apiServer->registerSender(id,[&socket](Susi::Util::Any & arg){
    	std::string msg = arg.toString();
    	socket.sendFrame(msg.data(), msg.length(), Poco::Net::WebSocket::FRAME_TEXT);        
    });
    
    char buffer[4096];
	int flags;
	size_t n;

	do {
		n = socket.receiveFrame(buffer, sizeof(buffer), flags);

		if(n > 0) {			
			std::string str(buffer, n);
			Susi::Util::Any packet = Susi::Util::Any::fromString(str);
			apiServer->onMessage(id,packet);   			
		}
	}
	while (n > 0 || (flags & Poco::Net::WebSocket::FRAME_OP_BITMASK) != Poco::Net::WebSocket::FRAME_OP_CLOSE);

}