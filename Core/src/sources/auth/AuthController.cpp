#include "auth/AuthController.h"

void Susi::Auth::Controller::setup() {
    auto db = _dbManager->getDatabase( this->_dbIdentifier );
    try {
        db->query( "SELECT id,username,password,authlevel FROM users" );
    } catch(const std::exception & e) {
        Susi::Logger::debug("AuthDB not ready!");
        db->query( "CREATE TABLE users(id INTEGER PRIMARY KEY AUTOINCREMENT, username VARCHAR, password VARCHAR, authlevel INTEGER);");
        db->query( "INSERT INTO users VALUES(0,'root','toor',0);");
    }

}

bool Susi::Auth::Controller::login( std::string sessionID, std::string username, std::string password ) {
    Susi::Logger::debug("handle login request");
    if( this->isLoggedIn( sessionID ) == false ) {
        Susi::Logger::debug("user is not allready logged in");
        auto db = _dbManager->getDatabase( this->_dbIdentifier );

        Susi::Util::Any result = db->query( "SELECT authlevel FROM users WHERE username=\'"+username+"\' AND password=\'"+password+"\'" );

        if( result.size()==0 ) {
            Susi::Logger::debug("username and password does not match");
            return false;
        }
        else {
            Susi::Logger::debug("Login success: "+result.toJSONString());
            _sessionManager->setSessionAttribute( sessionID, "User", Susi::Util::Any::Object( {{"username",username}} ) );
            _sessionManager->setSessionAttribute( sessionID, "authlevel", result[0]["authlevel"]);
            return true;
        }
    }
    else {
        return true; // allready logged in
    }

}

bool Susi::Auth::Controller::logout( std::string sessionID ) {
    _sessionManager->removeSessionAttribute( sessionID, "authlevel" );
    return _sessionManager->removeSessionAttribute( sessionID, "User" );
}

bool Susi::Auth::Controller::isLoggedIn( std::string sessionID ) {
    Susi::Util::Any user = _sessionManager->getSessionAttribute( sessionID, "User" );
    return !( user.isNull() );
}

std::string Susi::Auth::Controller::getUsername( std::string sessionID ) {
    Susi::Util::Any user = _sessionManager->getSessionAttribute( sessionID, "User" );

    if( user.isNull() ) {
        return "";
    }
    else {
        std::string username = user["username"];
        return username;
    }
}