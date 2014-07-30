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

#include "webstack/WebSocketRequestHandler.h"

void Susi::WebSocketRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request,
                       							Poco::Net::HTTPServerResponse& response) {
	Poco::Net::WebSocket socket(request,response);
	std::shared_ptr<Susi::Connection> connection(new Susi::WebSocketConnection(socket));
	Poco::Net::NameValueCollection cookies;
	request.getCookies(cookies);
	std::string id = cookies["susisession"];
	connection->setSessionID(id);
	Susi::ConnectionHandler::handleConnection(connection);		
}

