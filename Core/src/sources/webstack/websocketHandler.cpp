#include "webstack/OnionHttpServerComponent.h"

struct wsUserData {
	Susi::Webstack::HttpServerComponent *server;
	std::string connID;
	Susi::Api::JSONStreamCollector *collector;
};

onion_connection_status websocketDataHandler(void *data, onion_websocket *ws, size_t data_ready_len){
	auto *userData = static_cast<wsUserData*>(data);
	char tmp[1024];
	if (data_ready_len>sizeof(tmp))
		data_ready_len=sizeof(tmp)-1;
	
	int len=onion_websocket_read(ws, tmp, data_ready_len);
	if (len<=0 || (len == 2 && (uint8_t)tmp[0] == 3 && (uint8_t)tmp[1] == 233)){
		LOG(ERROR) << "Error reading data:" << strerror(errno);
		onion_websocket_free(ws);
		return OCS_CLOSE_CONNECTION;
	}
	std::string chunk{tmp,uint32_t(len)};
	userData->collector->collect(chunk);
	return OCS_NEED_MORE_DATA;
}

onion_connection_status Susi::Webstack::HttpServerComponent::websocketHandler(Onion::Request &req, Onion::Response &res){
	std::string sessionID = sessionHandling(req,res);
	LOG(DEBUG) << "new websocket connection!";
	onion_websocket * ws = onion_websocket_new(req.c_handler(),res.c_handler());
	if(!ws){
		LOG(ERROR) << "cant establish websocket connection";
		return OCS_PROCESSED;
	}
	std::string connID = std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
	auto collector = new Susi::Api::JSONStreamCollector{ [this,connID](std::string message){
		auto packet = Susi::Util::Any::fromJSONString(message);
		std::string id = connID;
		_apiServer->onMessage(id,packet);
	}};
	auto deleter = [](void *data){
		LOG(DEBUG) << "delete user data";
		auto *userData = static_cast<wsUserData*>(data);
		delete userData->collector;
		userData->server->apiServer()->onClose(userData->connID);
		delete userData;
	};
	auto * userData = new wsUserData{
		this,
		connID,
		collector
	};
	_sessionManager->addAlias(connID,sessionID);
	_apiServer->onConnect(connID);
	_apiServer->registerSender(connID,[ws](Susi::Util::Any & packet){
		std::string data = packet.toJSONString()+"\n";
		onion_websocket_write(ws,data.c_str(),data.size());
	});
	onion_websocket_set_userdata(ws, userData, deleter);
	onion_websocket_set_callback(ws, websocketDataHandler);

	return OCS_WEBSOCKET;
}