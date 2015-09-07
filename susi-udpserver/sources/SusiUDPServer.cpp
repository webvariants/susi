#include "susi/SusiUDPServer.h"

Susi::SusiUDPServer::SusiUDPServer(std::string addr,short port, std::string key, std::string cert, unsigned short udpPort) : 
	_susi{new Susi::SusiClient{addr,port,key,cert}},
	_socket{_io_service, boost::asio::ip::udp::endpoint{boost::asio::ip::udp::v4(),udpPort}} {
		_runloop = std::move(std::thread{[this](){
			_io_service.run();
		}});
		do_receive();
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
					auto event = _susi->createEvent(doc["data"]);
					auto endpoint = _sender_endpoint;
					_susi->publish(std::move(event),[this,endpoint](Susi::SharedEventPtr event){
						do_send(event->toString(), _sender_endpoint);
					});
				}
			}
		}
		do_receive();
	});
}

void Susi::SusiUDPServer::do_send(const std::string & message, boost::asio::ip::udp::endpoint receiver) {
	_socket.async_send_to(
		boost::asio::buffer(message.c_str(), message.size()), receiver,
		[this](boost::system::error_code /*ec*/, std::size_t /*bytes_sent*/){});
}