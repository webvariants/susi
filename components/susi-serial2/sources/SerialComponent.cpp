#include "susi/SerialComponent.h"

Susi::SerialComponent::SerialComponent(Susi::SusiClient & susi, BSON::Value & config) :
  _susi{susi}, _config{config} {

    _susi.registerProcessor("serial::write", [this](Susi::EventPtr event){
        auto & payload = event->payload;
        std::string id = payload["id"];
        std::string msg = payload["msg"];
        auto & port = _sessions[id];
        int size = port.port->write_some(boost::asio::buffer(msg));
        payload["bytes_written"] = size;
        _susi.ack(std::move(event));
    });
}

void Susi::SerialComponent::initPorts(){
    auto ports = _config["ports"].getArray();
    for(auto & port : ports){
        auto id = port["id"].getString();
        auto portname = port["port"].getString();
        auto baudrate = port["baudrate"].getInt64();
        initPort(id, portname, baudrate);
    }
}

void Susi::SerialComponent::join(){
    _susi.join();
}

void Susi::SerialComponent::initPort(const std::string & id, const std::string & portname, int baudrate){
    std::cout << "init serial port "<<id<<std::endl;
    std::shared_ptr<boost::asio::serial_port> serialPort{ new boost::asio::serial_port{_ioservice}};
    serialPort->open(portname);
    serialPort->set_option(boost::asio::serial_port::baud_rate(baudrate));
    serialPort->set_option(boost::asio::serial_port::parity(boost::asio::serial_port::parity::none));
    serialPort->set_option(boost::asio::serial_port::character_size(boost::asio::serial_port::character_size(8)));
    serialPort->set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::one));
    serialPort->set_option(boost::asio::serial_port::flow_control(boost::asio::serial_port::flow_control::none));
    _sessions.insert({id,Susi::SerialComponent::SerialSession{serialPort,this}});
    Susi::SerialComponent::SerialSession & port = _sessions[id];
    port.do_read();
}
