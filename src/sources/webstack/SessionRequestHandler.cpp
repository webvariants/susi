#include "webstack/SessionRequestHandler.h"
#include "world/World.h"

void Susi::SessionRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request,
                       Poco::Net::HTTPServerResponse& response){
	std::string id = "";
	Susi::Logger::debug("starting SessionRequestHandler");
	try{
		Poco::Net::NameValueCollection cookies;
		request.getCookies(cookies);
		id = cookies["susisession"];
		if(!_sessionManager->checkSession(id)){
			Susi::Logger::info("No valid session");
			Poco::Timestamp now;
			id = std::to_string(now.epochMicroseconds());
			_sessionManager->updateSession(id);
			response.addCookie(Poco::Net::HTTPCookie{"susisession",id});
			request.add("susisession",id);
		}else{
			request.add("susisession",id);
		}
	}catch(const std::exception & e){
		Susi::Logger::error("failed in session-request-handler");
		Poco::Timestamp now;
		id = std::to_string(now.epochMicroseconds());
		_sessionManager->updateSession(id);
		auto cookie = Poco::Net::HTTPCookie{"susisession",id};
		cookie.setPath("/");
		response.addCookie(cookie);
		request.add("susisession",id);
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
