#include "susi/SusiClient.h"
#include "susi/LineFramer.h"
#include <string> 
#include <iostream> 
#include <boost/asio.hpp> 
#include <boost/system/system_error.hpp> 

class SusiSerial {

public:
    SusiSerial(std::string addr,short port, std::string key, std::string cert);
    void join();
    void initPort(const std::string & id, const std::string & portname, int baudrate);

    void run(){
        _ioservice.run();
    }

protected:

    struct SerialSession {
        SerialSession(){}
        SerialSession(std::shared_ptr<boost::asio::serial_port> p, SusiSerial * serial) : 
          port{p}, 
          parent{serial},
          lineframer{[this](std::string & message){
            auto event = parent->_susi->createEvent("serial::data");
            std::cout << message << std::endl;
            event->payload = message;
            parent->_susi->publish( std::move(event) );
        }} {}
        std::shared_ptr<boost::asio::serial_port> port;
        SusiSerial * parent;
        Susi::LineFramer lineframer;
        char data[4096];
        void do_read(){
            port->async_read_some(boost::asio::buffer(data, sizeof data), [this](boost::system::error_code ec, std::size_t length){
                //std::cout << "got data!" << std::endl;
                if (!ec){
                    std::cout << "\033[1;32m Data: " << std::string{data,length} << " with length:" << length << "\033[0m" << std::endl;
                    lineframer.collect(data,length);
                }else{
                    std::this_thread::sleep_for(std::chrono::milliseconds{500});
                    std::cout <<"\033[1;31m Error: "<< ec.message() << "\033[0m" << std::endl;
                }
                do_read();
            });
        }
    };

    std::map<std::string, SerialSession> _sessions;
    std::shared_ptr<Susi::SusiClient> _susi;
    boost::asio::io_service _ioservice;
};

