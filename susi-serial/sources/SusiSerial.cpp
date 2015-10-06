#include "susi/SusiSerial.h"


SusiSerial::SusiSerial(std::string addr,short port, std::string key, std::string cert) :
  _susi{new Susi::SusiClient{addr,port,key,cert}} {
    _susi->registerProcessor("serial::write", [this](Susi::EventPtr event){
        auto & payload = event->payload;
        std::string id = payload["id"];
        std::string msg = payload["msg"];
        
        auto & port = _ports[id];
        int size = port.port->write_some(boost::asio::buffer(msg));
        payload["bytes_written"] = size;
        _susi->ack(std::move(event));
    });
   
}

void SusiSerial::join(){
    _susi->join();
}
void SusiSerial::doRead(SusiSerial::SerialPortData serialPort){
    // std::cout << "doRead" << std::endl;
    serialPort.port->async_read_some(boost::asio::buffer(serialPort.data, sizeof serialPort.data), [this,serialPort](boost::system::error_code ec, std::size_t length){
        //std::cout << "got data!" << std::endl;
        if (!ec){
            std::cout << "\033[1;32m Data: " << std::string{serialPort.data,length} << " with length:" << length << "\033[0m" << std::endl;
            auto event = _susi->createEvent("serial::data");
            event->payload = std::string{serialPort.data,sizeof serialPort}+"\n";
            _susi->publish( std::move(event) );
        }else{
            std::this_thread::sleep_for(std::chrono::milliseconds{100});
            //std::cout <<"\033[1;31m Error: "<< ec.message() << "\033[0m" << std::endl;
        }
        doRead(serialPort);
    });
}

void SusiSerial::initPort(const std::string & id, const std::string & portname, int baudrate){
    std::shared_ptr<boost::asio::serial_port> serialPort{ new boost::asio::serial_port{_ioservice}};
    serialPort->open(portname); 
    serialPort->set_option(boost::asio::serial_port::baud_rate(baudrate)); 
    serialPort->set_option(boost::asio::serial_port::parity(boost::asio::serial_port::parity::none)); 
    serialPort->set_option(boost::asio::serial_port::character_size(boost::asio::serial_port::character_size(8))); 
    serialPort->set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::one)); 
    serialPort->set_option(boost::asio::serial_port::flow_control(boost::asio::serial_port::flow_control::none));
    _ports.insert({id,SusiSerial::SerialPortData{serialPort}});
    SusiSerial::SerialPortData & port = _ports[id];
    doRead(port);
}

