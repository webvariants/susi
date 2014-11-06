#include "webstack/SessionRequestHandler.h"

void Susi::SessionRequestHandler::handleRequest( Poco::Net::HTTPServerRequest& request,
        Poco::Net::HTTPServerResponse& response ) {
    std::string id = "";
    LOG(DEBUG) <<  "starting SessionRequestHandler" ;
    try {
        Poco::Net::NameValueCollection cookies;
        request.getCookies( cookies );
        id = cookies["susisession"];
        LOG(INFO) <<  "sessionid: "+id ;
        if( !_sessionManager->checkSession( id ) ) {
            LOG(INFO) <<  "No valid session" ;
            auto oldCookie = Poco::Net::HTTPCookie {"susisession",id};
            oldCookie.setMaxAge( 0 );
            response.addCookie( oldCookie );
            Poco::Timestamp now;
            id = std::to_string( now.epochMicroseconds() );
            _sessionManager->updateSession( id );
            auto cookie = Poco::Net::HTTPCookie {"susisession",id};
            cookie.setPath( "/" );
            response.addCookie( cookie );
        }
    }
    catch( const std::exception & e ) {
        LOG(DEBUG) <<  "no session found, add new one" ;
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
    }
    try {
        defaultHandler->handleRequest( request,response );
    }
    catch( const std::exception & e ) {
        std::string msg = "error in http handler: ";
        msg += e.what();
        LOG(ERROR) <<  msg ;
    }
    LOG(DEBUG) <<  "finished in SessionRequestHandler" ;
}
