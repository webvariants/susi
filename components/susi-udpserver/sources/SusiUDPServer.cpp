#include "susi/UDPServerComponent.h"

Susi::UDPServerComponent::UDPServerComponent(Susi::SusiClient & susi, BSON::Value & config) :
	_susi{susi},
	_socket{_io_service, boost::asio::ip::udp::endpoint{boost::asio::ip::udp::v6(),static_cast<unsigned short>(config["port"].getInt64())}} {
		do_receive();
		_runloop = std::move(std::thread{[this](){
			_io_service.run();
		}});
}

void Susi::UDPServerComponent::stop() {
	_io_service.stop();
}

void Susi::UDPServerComponent::join() {
	if(_runloop.joinable())_runloop.join();
}

Susi::UDPServerComponent::~UDPServerComponent() {
	stop();
	join();
}

void Susi::UDPServerComponent::do_receive() {
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

void Susi::UDPServerComponent::do_send(const std::string & message, boost::asio::ip::udp::endpoint receiver) {
	_socket.async_send_to(
		boost::asio::buffer(message.c_str(), message.size()), receiver,
		[this](boost::system::error_code ec, std::size_t /*bytes_sent*/){
			if(ec){
				std::cout<<"error while sending packet: "<<ec<<std::endl;
			}
		});
}

void Susi::UDPServerComponent::handlePublish(BSON::Value & eventData, boost::asio::ip::udp::endpoint & sender){
	auto event = _susi.createEvent(eventData);
	auto endpoint = _sender_endpoint;
	std::cout<<"publish event with topic "<<event->topic<<" for "<<sender<<std::endl;
	_susi.publish(std::move(event),[this,endpoint](Susi::SharedEventPtr event){
		std::cout<<"forward ack with topic "<<event->topic<<" to "<<endpoint<<std::endl;
		BSON::Value packet = BSON::Object{
			{"type","ack"},
			{"data",event->toAny()}
		};
		//do_send(packet->toJSON()+"\n", _sender_endpoint); //BAD ERROR ;)
		do_send(packet.toJSON()+"\n", endpoint);
	});
}

void Susi::UDPServerComponent::handleRegister(BSON::Value & eventData, boost::asio::ip::udp::endpoint & sender){
	auto topic = eventData["topic"].getString();
	auto & sendersRegistrations = _registrations[sender];
	if(sendersRegistrations.count(topic) == 1) {
		BSON::Value result = BSON::Object{
			{"type","error"},
			{"data","you are already registered for the topic "+topic}
		};
		do_send(result.toJSON()+"\n",sender);
	}else{
		auto endpoint = _sender_endpoint;

		std::cout<<"register consumer for topic "<<topic<<" for "<<endpoint<<std::endl;
		sendersRegistrations[topic] = _susi.registerConsumer(topic,[this,endpoint](Susi::SharedEventPtr event){
			std::cout<<"forward event with topic "<<event->topic<<" to "<<endpoint<<std::endl;
			BSON::Value packet = BSON::Object{
				{"type","event"},
				{"data",event->toAny()}
			};
			do_send(packet.toJSON()+"\n", endpoint);
		});
	}
}

void Susi::UDPServerComponent::handleUnregister(BSON::Value & eventData, boost::asio::ip::udp::endpoint & sender){
	auto topic = eventData["topic"].getString();
	std::cout<<"unregister consumer for topic "<<topic<<" for "<<sender<<std::endl;
	auto & sendersRegistrations = _registrations[sender];
	if(sendersRegistrations.count(topic) == 0) {
		BSON::Value result = BSON::Object{
			{"type","error"},
			{"data","you are not registered for the topic "+topic}
		};
		do_send(result.toJSON()+"\n",sender);
	}else{
		sendersRegistrations.erase(topic);
		if(sendersRegistrations.size() == 0){
			_registrations.erase(sender);
		}
	}
}
