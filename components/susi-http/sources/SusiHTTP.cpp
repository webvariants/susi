#include "susi/SusiHTTP.h"

SusiHTTP::SusiHTTP(std::string addr,unsigned short port, std::string key, std::string cert, unsigned short httpPort) :
  susi_{new Susi::SusiClient{addr,(short)port,key,cert}},
  server_{httpPort, 4, cert, key} {

    auto& ws=server_.endpoint["^/ws/?$"];

    ws.onmessage=[this](std::shared_ptr<WssServer::Connection> connection, std::shared_ptr<WssServer::Message> message) {
        onMessage(connection,message);
    };

    ws.onopen=[this](std::shared_ptr<WssServer::Connection> connection) {
        onOpen(connection);
    };

    //See RFC 6455 7.4.1. for status codes
    ws.onclose=[this](std::shared_ptr<WssServer::Connection> connection, int status, const std::string& reason) {
        onClose(connection,status,reason);
    };

    //See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
    ws.onerror=[this](std::shared_ptr<WssServer::Connection> connection, const boost::system::error_code& ec) {
        onError(connection,ec);
    };

    runloop_ = std::move(std::thread{[this](){
        server_.start();
    }});

}


void SusiHTTP::onMessage(std::shared_ptr<WssServer::Connection> connection, std::shared_ptr<WssServer::Message> message){
    auto message_str=message->string();

    std::cout << "Server: Message received: \"" << message_str << "\" from " << (size_t)connection.get() << std::endl;

    std::cout << "Server: Sending message \"" << message_str <<  "\" to " << (size_t)connection.get() << std::endl;

    auto send_stream=std::make_shared<WssServer::SendStream>();
    *send_stream << message_str;
    //server_.send is an asynchronous function
    server_.send(connection, send_stream, [](const boost::system::error_code& ec){
        if(ec) {
            std::cout << "Server: Error sending message. " <<
            //See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
                    "Error: " << ec << ", error message: " << ec.message() << std::endl;
        }
    });
}

void SusiHTTP::onOpen(std::shared_ptr<WssServer::Connection> connection){
    std::cout << "Server: Opened connection " << (size_t)connection.get() << std::endl;
}

void SusiHTTP::onClose(std::shared_ptr<WssServer::Connection> connection, int status, const std::string& reason){
    std::cout << "Server: Closed connection " << (size_t)connection.get() << " with status code " << status << std::endl;
}

void SusiHTTP::onError(std::shared_ptr<WssServer::Connection> connection, const boost::system::error_code& ec){
    std::cout << "Server: Error in connection " << (size_t)connection.get() << ". " <<
                 "Error: " << ec << ", error message: " << ec.message() << std::endl;
}


void SusiHTTP::join(){
	susi_->join();
}
