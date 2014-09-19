#include "webstack/SessionRequestHandler.h"

void Susi::SessionRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request,
                       Poco::Net::HTTPServerResponse& response){
	std::string id = "";
	Susi::Logger::debug("starting SessionRequestHandler");
	try{
		Poco::Net::NameValueCollection cookies;
		request.getCookies(cookies);
		id = cookies["susisession"];
		Susi::Logger::info("sessionid: "+id);
		if(!_sessionManager->checkSession(id)){
			Susi::Logger::info("No valid session");
			auto oldCookie = Poco::Net::HTTPCookie{"susisession",id};
			oldCookie.setMaxAge(0);
			response.addCookie(oldCookie);
			Poco::Timestamp now;
			id = std::to_string(now.epochMicroseconds());
			_sessionManager->updateSession(id);
			auto cookie = Poco::Net::HTTPCookie{"susisession",id};
			cookie.setPath("/");
			response.addCookie(cookie);
		}
	}catch(const std::exception & e){
		Susi::Logger::debug("no session found, add new one");
		Poco::Timestamp now;
		id = std::to_string(now.epochMicroseconds());
		_sessionManager->updateSession(id);
		auto cookie = Poco::Net::HTTPCookie{"susisession",id};
		cookie.setPath("/");
		response.addCookie(cookie);
	}
	try{
		defaultHandler->handleRequest(request,response);
	}catch(const std::exception & e){
		std::string msg = "error in http handler: ";
		msg += e.what();
		Susi::Logger::error(msg);
	}
	Susi::Logger::debug("finished in SessionRequestHandler");
}
