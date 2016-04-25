#include "susi/SusiClient.h"
#include "susi/LineFramer.h"
#include <string>
#include <iostream>
#include "Serial.h"
// #include <boost/asio.hpp>
#include <boost/system/system_error.hpp>

namespace Susi {

    class SerialComponent {

    public:
        SerialComponent(Susi::SusiClient & susi, BSON::Value & config);
        void join();
        void initPorts();
        void initPort(const std::string & id, const std::string & portname, int baudrate, int char_size, int parity);

        void run(){
            _ioservice.run();
        }

    protected:

        struct SerialSession {
            SerialSession(){}
            SerialSession(std::shared_ptr<boost::asio::serial_port> p, SerialComponent * serial) :
              port{p},
              parent{serial},
              lineframer{[this](std::string & message){
                auto event = parent->_susi.createEvent("serial::data");
                std::cout << "\033[1;32m Data: " << message << "\033[0m" << std::endl;
    	    event->payload = message;
                parent->_susi.publish( std::move(event) );
            }} {}
            std::map<std::string> port;
            SerialComponent * parent;
            Susi::LineFramer lineframer;
            char data[4096];
            void do_read(){
                port->async_read_some(boost::asio::buffer(data, sizeof data), [this](boost::system::error_code ec, std::size_t length){
                    if (!ec){
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
        Susi::SusiClient & _susi;
        BSON::Value & _config;
        boost::asio::io_service _ioservice;
    };

}
