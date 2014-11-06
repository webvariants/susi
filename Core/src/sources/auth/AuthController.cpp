#include "auth/AuthController.h"


std::string Susi::Auth::Controller::generateSalt() {
    std::random_device randomDevice;
    std::string possibleChars = "0123456789abcdefghijklmnopqrstuvwxyz";
    int max = possibleChars.size()-1;
    std::uniform_int_distribution<int> d {0, max};
    std::string salt = "";
    size_t saltLen = 32;
    for( size_t i=0; i<saltLen; ++i ) {
        salt += possibleChars[d( randomDevice )];
    }
    return salt;
}

void Susi::Auth::Controller::setup() {
    auto db = _dbManager->getDatabase( this->_dbIdentifier );
    try {
        db->query( "SELECT id,username,password,salt,authlevel FROM users;" );
    }
    catch( const std::exception & e ) {
        LOG(DEBUG) <<  "AuthDB not ready!" ;
        SHA3 hasher;
        db->query( "CREATE TABLE users(id INTEGER PRIMARY KEY AUTOINCREMENT, username VARCHAR UNIQUE, password VARCHAR, salt VARCHAR, authlevel INTEGER);" );
        if( !addUser( "root","toor",0 ) ) {
            throw std::runtime_error {"this should not happen"};
        }
    }
}

bool Susi::Auth::Controller::addUser( std::string username, std::string password , char authlevel ) {
    auto db = _dbManager->getDatabase( this->_dbIdentifier );
    try {
        std::string authlevelStr = std::to_string( ( int )authlevel );
        std::string salt = generateSalt();
        SHA3 hasher;
        std::string pwHash = hasher( password+salt );
        if( !checkForSqlSafety( username ) ) {
            LOG(ERROR) <<  "username seems to be an sql injection: "+username ;
            return false;
        }
        db->query( "INSERT INTO users(username,password,salt,authlevel) "
                   "VALUES('"+username+"','"+pwHash+"','"+salt+"',"+authlevelStr+");" );
    }
    catch( const std::exception & e ) {
        return false;
    }
    return true;
}

bool Susi::Auth::Controller::updateUser( std::string username, std::string password , char authlevel ) {
    auto db = _dbManager->getDatabase( this->_dbIdentifier );
    try {
        std::string authlevelStr = std::to_string( ( int )authlevel );
        std::string salt = generateSalt();
        SHA3 hasher;
        std::string pwHash = hasher( password+salt );
        if( !checkForSqlSafety( username ) ) {
            LOG(ERROR) <<  "username seems to be an sql injection: "+username ;
            return false;
        }
        db->query( "UPDATE INTO users(password,salt,authlevel) "
                   "VALUES('"+pwHash+"','"+salt+"',"+authlevelStr+") WHERE username='"+username+"';" );
    }
    catch( const std::exception & e ) {
        return false;
    }
    return true;
}

bool Susi::Auth::Controller::delUser( std::string username ) {
    if( !checkForSqlSafety( username ) ) {
        return false;
    }
    try {
        auto db = _dbManager->getDatabase( this->_dbIdentifier );
        db->query( "DELETE FROM users WHERE username='"+username+"'" );
    }
    catch( ... ) {
        return false;
    }
    return true;
}

bool Susi::Auth::Controller::login( std::string sessionID, std::string username, std::string password ) {
    LOG(DEBUG) <<  "handle login request" ;
    if( this->isLoggedIn( sessionID ) == false ) {
        LOG(DEBUG) <<  "user is not allready logged in" ;
        auto db = _dbManager->getDatabase( this->_dbIdentifier );
        if( !checkForSqlSafety( username ) ) {
            return false;
        }
        Susi::Util::Any result = db->query( "SELECT authlevel,salt,password FROM users WHERE username='"+username+"';" );
        if( result.size()==0 ) {
            LOG(DEBUG) <<  "username and password does not match" ;
            return false;
        }
        std::string salt = result[0]["salt"];
        SHA3 hasher;
        std::string pwHash = hasher( password+salt );
        std::string expectedHash = result[0]["password"];
        if( pwHash == expectedHash ) {
            LOG(DEBUG) <<  "Login success: "+result.toJSONString() ;
            _sessionManager->setSessionAttribute( sessionID, "User", Susi::Util::Any::Object( {{"username",username}} ) );
            _sessionManager->setSessionAttribute( sessionID, "authlevel", result[0]["authlevel"] );
            return true;
        }
        return false;
    }
    return false; // allready logged in
}

bool Susi::Auth::Controller::logout( std::string sessionID ) {
    if( !isLoggedIn( sessionID ) ) {
        return false;
    }
    _sessionManager->removeSessionAttribute( sessionID, "authlevel" );
    _sessionManager->removeSessionAttribute( sessionID, "User" );
    return true;
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