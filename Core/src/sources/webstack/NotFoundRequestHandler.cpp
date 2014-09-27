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

#include "webstack/NotFoundRequestHandler.h"

void Susi::NotFoundRequestHandler::handleRequest( Poco::Net::HTTPServerRequest& request,
        Poco::Net::HTTPServerResponse& response ) {
    response.setChunkedTransferEncoding( true );
    response.setContentType( "text/html" );
    response.setStatus( Poco::Net::HTTPServerResponse::HTTPStatus::HTTP_NOT_FOUND );
    std::ostream& ostr = response.send();
    ostr << "<html><head><title>SUSI</title></head> ";
    ostr << "<body><p style=\"text-align: center;font-size: 48px;\"></br></br></br>";
    ostr << "(404) Not found.";
    ostr << "</p></body></html>";
}
