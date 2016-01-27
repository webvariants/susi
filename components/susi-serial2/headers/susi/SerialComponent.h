#include "susi/SusiClient.h"
#include "susi/LineFramer.h"
#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/system/system_error.hpp>

namespace Susi {

    class SerialComponent {

    public:
        SerialComponent(Susi::SusiClient & susi, BSON::Value & config);
        void join();
        void initPorts();
        void initPort(const std::string & id, const std::string & portname, int baudrate);

        void run(){
            _ioservice.run();
        }

    protected:

        struct SerialSession {
            SerialSession(){}
            SerialSession(std::shared_ptr<boost::asio::serial_port> p, SerialComponent * serial) :
            port{p},
            parent{serial},
            lineframer{[this](std::string & message){this->onLine(message);}}
             {}
            void onLine(std::string & message){
                auto event = parent->_susi.createEvent("serial::data");
                BSON::Value data = BSON::Value::fromJSON(message);
                std::cout << data.toJSON() << std::endl;
                event->payload = data["payload"];
                event->topic = data["topic"];
                parent->_susi.publish( std::move(event) );
            }
            std::shared_ptr<boost::asio::serial_port> port;
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
