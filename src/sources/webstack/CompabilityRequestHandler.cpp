#include "webstack/CompabilityRequestHandler.h"
#include "world/World.h"
#include "logger/Logger.h"

void Susi::CompabilityRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request,
                       Poco::Net::HTTPServerResponse& response){
	try{
		Susi::debug("got compability request");
		std::string sessionID = request.get("susisession");
		if(request.getMethod() == "POST"){
			Poco::JSON::Parser parser;
			Poco::Dynamic::Var body = parser.parse(request.stream());
			Poco::JSON::Object::Ptr obj = body.extract<Poco::JSON::Object::Ptr>();
			if(!obj->has("type")){
				response.setStatus(Poco::Net::HTTPServerResponse::HTTPStatus::HTTP_NOT_ACCEPTABLE);
				response.send().flush();
				return;	
			}
			std::string type = obj->getValue<std::string>("type");
			if(type == "publish"){
				std::string key = obj->getValue<std::string>("key");
				Susi::Event event(key);
				event.sessionID = sessionID;
				if(obj->has("payload")){
					event.payload = obj->get("payload");
				}
				if(obj->has("returnaddr")){
					event.returnAddr = obj->getValue<std::string>("returnaddr");
				}
				Susi::debug("publish event via compability layer");
				Susi::publish(event);
				response.setStatus(Poco::Net::HTTPServerResponse::HTTPStatus::HTTP_OK);
				response.send().flush();
				return;
			}else if(type == "subscribe"){
				std::string key = obj->getValue<std::string>("key");
				int id = Susi::subscribe(key,[sessionID](Susi::Event & event){
					world.sessionManager->pushSessionAttribute(sessionID,"events",event.toString());
				});
				Susi::debug("subscribe to "+key+" via compability layer");
				Susi::once("webstack::compability::unsubscribe::"+sessionID,[sessionID,key,id](Susi::Event & event){
					Susi::debug("unsubscribe requested, therefore unsubscribe "+sessionID+" from "+key);
					Susi::unsubscribe(key,id);
				});
				Susi::once("session::die::"+sessionID,[sessionID,key,id](Susi::Event & event){
					Susi::debug("session died, therefore unsubscribe "+sessionID+" from "+key);
					Susi::Event unsubEvent("webstack::compability::unsubscribe::"+sessionID);
					Susi::publish(unsubEvent);
				});
				response.setStatus(Poco::Net::HTTPServerResponse::HTTPStatus::HTTP_OK);
				response.send().flush();
				return;
			} else if(type == "unsubscribe"){
				std::string key = obj->getValue<std::string>("key");
				Susi::Event unsubEvent("webstack::compability::unsubscribe::"+sessionID);
				Susi::publish(unsubEvent);
				response.setStatus(Poco::Net::HTTPServerResponse::HTTPStatus::HTTP_OK);
				response.send().flush();
				return;
			} else if(type == "get"){
				response.setChunkedTransferEncoding(true);
				response.setContentType("text/json");
				response.setStatus(Poco::Net::HTTPServerResponse::HTTPStatus::HTTP_OK);
				Poco::Dynamic::Var events = world.sessionManager->getSessionAttribute(sessionID,"events");
				if (events.isEmpty()){
					auto & ostr = response.send();
					ostr << "[]";
					ostr.flush();
					return;
				}
				if (!events.isVector()) {
					std::ostream& ostr = response.send();
					ostr <<"["<< events.extract<std::string>() <<"]";
					ostr.flush();
				} else {
					std::ostream& ostr = response.send();
					ostr << "[";
					auto vec = events.extract<std::vector<Poco::Dynamic::Var>>();
					for(std::size_t i=0; i<vec.size(); i++){
						ostr << vec[i].extract<std::string>();
						if(i<vec.size()-1)ostr << ",";
					}
					ostr << "]";
					ostr.flush();
				}
				world.sessionManager->removeSessionAttribute(sessionID,"events");
				return;
			}
		}else{
			Susi::debug("compability request which is not POST");
			response.setStatus(Poco::Net::HTTPServerResponse::HTTPStatus::HTTP_METHOD_NOT_ALLOWED);
			response.send().flush();
			return;
		}
	}catch(const std::exception & e){
		std::string msg = "got exception in compability request: ";
		msg += e.what();
		Susi::debug(msg);
		response.setStatus(Poco::Net::HTTPServerResponse::HTTPStatus::HTTP_NOT_ACCEPTABLE);
		response.send().flush();
		return;
	}
}
