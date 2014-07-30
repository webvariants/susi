#include "auth/AuthController.h"
#include "world/World.h"

Susi::Auth::Controller::Controller(std::string db_identifier) {
	this->db_identifier = db_identifier;
}

bool Susi::Auth::Controller::login(std::string sessionID, std::string username, std::string password) {

	if(this->isLoggedIn(sessionID) == false) {
		auto db = world.dbManager->getDatabase(this->db_identifier);


		Poco::Dynamic::Var result = db->query("SELECT id FROM users WHERE username=\'"+username+"\' AND password=\'"+password+"\'");

		auto vec = result.extract<Poco::Dynamic::Vector>();
		if(vec.size()==0) {
			return false;
		} else {
		 	world.sessionManager->setSessionAttribute(sessionID, "User", Susi::Event::Payload({{"username",username}}));
			return true;
		}
	} else {
		return true; // allready logged in
	}

}

void Susi::Auth::Controller::logout(std::string sessionID) {
	world.sessionManager->removeSessionAttribute(sessionID, "User");
}

bool Susi::Auth::Controller::isLoggedIn(std::string sessionID) {
	Poco::Dynamic::Var user = world.sessionManager->getSessionAttribute(sessionID, "User");
	return !user.isEmpty();
}

std::string Susi::Auth::Controller::getUsername(std::string sessionID) {
	Poco::Dynamic::Var user = world.sessionManager->getSessionAttribute(sessionID, "User");

	if(user.isEmpty()) {
		return "";
	} else {
		return user["username"].convert<std::string>();	
	}
}