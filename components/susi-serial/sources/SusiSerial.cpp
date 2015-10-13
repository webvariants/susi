#include "susi/SusiSerial.h"


SusiSerial::SusiSerial(std::string addr,short port, std::string key, std::string cert) :
  _susi{new Susi::SusiClient{addr,port,key,cert}} {
    _susi->registerProcessor("serial::write", [this](Susi::EventPtr event){
        auto & payload = event->payload;
        std::string id = payload["id"];
        std::string msg = payload["msg"];
        
        auto & port = _sessions[id];
        int size = port.port->write_some(boost::asio::buffer(msg));
        payload["bytes_written"] = size;
        _susi->ack(std::move(event));
    });
   
}

void SusiSerial::join(){
    _susi->join();
}

void SusiSerial::initPort(const std::string & id, const std::string & portname, int baudrate){
    std::shared_ptr<boost::asio::serial_port> serialPort{ new boost::asio::serial_port{_ioservice}};
    serialPort->open(portname); 
    serialPort->set_option(boost::asio::serial_port::baud_rate(baudrate)); 
    serialPort->set_option(boost::asio::serial_port::parity(boost::asio::serial_port::parity::none)); 
    serialPort->set_option(boost::asio::serial_port::character_size(boost::asio::serial_port::character_size(8))); 
    serialPort->set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::one)); 
    serialPort->set_option(boost::asio::serial_port::flow_control(boost::asio::serial_port::flow_control::none));
    _sessions.insert({id,SusiSerial::SerialSession{serialPort,this}});
    SusiSerial::SerialSession & port = _sessions[id];
    port.do_read();
}

