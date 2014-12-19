#include "webstack/SessionRequestHandler.h"

void Susi::SessionRequestHandler::handleRequest( Poco::Net::HTTPServerRequest& request,
        Poco::Net::HTTPServerResponse& response ) {
    std::string id = "";
    LOG(DEBUG) <<  "starting SessionRequestHandler" ;
    try {
        Poco::Net::NameValueCollection cookies;
        request.getCookies( cookies );
        id = cookies["susisession"];
        if( !_sessionManager->checkSession( id ) ) {
            LOG(DEBUG) <<  "No valid session: " << id ;
            auto oldCookie = Poco::Net::HTTPCookie {"susisession",id};
            oldCookie.setMaxAge( 0 );
            response.addCookie( oldCookie );
            Poco::Timestamp now;
            id = std::to_string( now.epochMicroseconds() );
            _sessionManager->updateSession( id );
            auto cookie = Poco::Net::HTTPCookie {"susisession",id};
            cookie.setPath( "/" );
            response.addCookie( cookie );
            LOG(DEBUG) << "added session id " << id;
        }
    }
    catch( const std::exception & e ) {
        Poco::Timestamp now;
        id = std::to_string( now.epochMicroseconds() );
        _sessionManager->updateSession( id );
        auto cookie = Poco::Net::HTTPCookie {"susisession",id};
        cookie.setPath( "/" );
        response.addCookie( cookie );
        Poco::Net::NameValueCollection cookies;
        request.getCookies( cookies );
        cookies.add( "susisession",id );
        request.setCookies( cookies );
        LOG(DEBUG) <<  "no session found, added session id " << id ;
    }
    try {
        defaultHandler->handleRequest( request,response );
    }
    catch( const std::exception & e ) {
        std::string msg = "error in http handler: ";
        msg += e.what();
        LOG(ERROR) <<  msg ;
    }
}
