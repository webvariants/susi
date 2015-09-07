#include "susi/SusiUDPServer.h"

Susi::SusiUDPServer::SusiUDPServer(std::string addr,short port, std::string key, std::string cert, unsigned short udpPort) : 
	_susi{new Susi::SusiClient{addr,port,key,cert}},
	_socket{_io_service, boost::asio::ip::udp::endpoint{boost::asio::ip::udp::v6(),udpPort}} {
		do_receive();
		_runloop = std::move(std::thread{[this](){
			_io_service.run();
		}});
}

void Susi::SusiUDPServer::stop() {
	_io_service.stop();
}

void Susi::SusiUDPServer::join() {
	if(_runloop.joinable())_runloop.join();
}

Susi::SusiUDPServer::~SusiUDPServer() {
	stop();
	join();
}

void Susi::SusiUDPServer::do_receive() {
	_socket.async_receive_from(
	boost::asio::buffer(_packet, 1<<16), _sender_endpoint,
	[this](boost::system::error_code ec, std::size_t bytes_recvd) {
		if (!ec && bytes_recvd > 0) {
			std::string packetString{_packet,bytes_recvd};
			auto doc = BSON::Value::fromJSON(packetString);            
			if(doc["type"].isString() && doc["type"].getString() == "publish"){
				if(doc["data"].isObject() && doc["data"]["topic"].isString()){
					handlePublish(doc["data"],_sender_endpoint);
				}
			}else if(doc["type"].isString() && doc["type"].getString() == "register"){
				if(doc["data"].isObject() && doc["data"]["topic"].isString()){
					handleRegister(doc["data"],_sender_endpoint);
				}
			}else if(doc["type"].isString() && doc["type"].getString() == "unregister"){
				if(doc["data"].isObject() && doc["data"]["topic"].isString()){
					handleUnregister(doc["data"],_sender_endpoint);
				}
			} 
		}
		do_receive();
	});
}

void Susi::SusiUDPServer::do_send(const std::string & message, boost::asio::ip::udp::endpoint receiver) {
	_socket.async_send_to(
		boost::asio::buffer(message.c_str(), message.size()), receiver,
		[this](boost::system::error_code ec, std::size_t /*bytes_sent*/){
			if(ec){
				std::cout<<"error while sending packet: "<<ec<<std::endl;
			}
		});
}

void Susi::SusiUDPServer::handlePublish(BSON::Value & eventData, boost::asio::ip::udp::endpoint & sender){
	auto event = _susi->createEvent(eventData);
	auto endpoint = _sender_endpoint;
	_susi->publish(std::move(event),[this,endpoint](Susi::SharedEventPtr event){
		BSON::Value packet = BSON::Object{
			{"type","ack"},
			{"data",event->toAny()}
		};
		//do_send(packet->toJSON(), _sender_endpoint); //BAD ERROR ;)
		do_send(packet.toJSON(), endpoint);
	});
}

void Susi::SusiUDPServer::handleRegister(BSON::Value & eventData, boost::asio::ip::udp::endpoint & sender){
	auto topic = eventData["topic"].getString();
	auto & sendersRegistrations = _registrations[sender];
	if(sendersRegistrations.count(topic) == 1) {
		BSON::Value result = BSON::Object{
			{"type","error"},
			{"data","you are already registered for the topic "+topic}
		};
		do_send(result.toJSON(),sender);
	}else{
		auto endpoint = _sender_endpoint;
		sendersRegistrations[topic] = _susi->registerConsumer(topic,[this,endpoint](Susi::SharedEventPtr event){
			BSON::Value packet = BSON::Object{
				{"type","event"},
				{"data",event->toAny()}
			};
			do_send(packet.toJSON(), endpoint);
		});
	}
}

void Susi::SusiUDPServer::handleUnregister(BSON::Value & eventData, boost::asio::ip::udp::endpoint & sender){
	auto topic = eventData["topic"].getString();
	auto & sendersRegistrations = _registrations[sender];
	if(sendersRegistrations.count(topic) == 0) {
		BSON::Value result = BSON::Object{
			{"type","error"},
			{"data","you are not registered for the topic "+topic}
		};
		do_send(result.toJSON(),sender);
	}else{
		sendersRegistrations.erase(topic);
		if(sendersRegistrations.size() == 0){
			_registrations.erase(sender);
		}
	}
}

