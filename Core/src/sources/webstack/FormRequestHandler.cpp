#include "webstack/FormRequestHandler.h"

void Susi::FormRequestHandler::handleRequest( Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response ) {

    Susi::MyPartHandler partHandler( this->_uploadDirectory );
    Poco::Net::HTMLForm form( request, request.stream(), partHandler );

    response.setChunkedTransferEncoding( true );
    response.setContentType( "text/html" );

    std::ostream& ostr = response.send();

    if( !partHandler.name().empty() )
    {
        response.setStatus( Poco::Net::HTTPServerResponse::HTTPStatus::HTTP_OK );

        ostr << "<html><head><title>SUSI</title></head> ";
        ostr << "<body>";
        ostr << "<h2>Upload</h2>\n";
        ostr << "<p style=\"text-align: center;font-size: 48px;\"></br></br></br>";
        ostr << "Name: " << partHandler.name() << "<br>\n";
        ostr << "File Name: " << partHandler.fileName() << "<br>\n";
        ostr << "Type: " << partHandler.contentType() << "<br>\n";
        ostr << "Size: " << partHandler.length() << "<br>\n";
        ostr << "</p></body></html>";

    }
    else {
        response.setStatus( Poco::Net::HTTPServerResponse::HTTPStatus::HTTP_NOT_FOUND );

        ostr << "<html><head><title>SUSI</title></head> ";
        ostr << "<body><p style=\"text-align: center;font-size: 48px;\"></br></br></br>";
        ostr << "(404) Upload Failed";
        ostr << "</p></body></html>";
    }
}