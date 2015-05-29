/*
 * Copyright (c) 2015, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 * 
 * @author: Tino Rusch (tino.rusch@webvariants.de)
 */

#include "susi/SSLClient.h"

Susi::SSLClient::SSLClient(std::string host, short port){
    endpoint_iterator_ = resolver_.resolve({host,std::to_string(port)});
    do_connect();
    runloop_ = std::move(std::thread{[this](){io_service_.run();}});
}

Susi::SSLClient::SSLClient(std::string host, short port, std::string keyfile, std::string certfile){
    context_.use_private_key_file(keyfile,boost::asio::ssl::context::pem);
    context_.use_certificate_file(certfile,boost::asio::ssl::context::pem);
    socket_ = std::make_shared<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>(io_service_,context_);
    endpoint_iterator_ = resolver_.resolve({host,std::to_string(port)});
    do_connect();
    runloop_ = std::move(std::thread{[this](){io_service_.run();}});
}

void Susi::SSLClient::join(){
    if(runloop_.joinable())runloop_.join();
}

Susi::SSLClient::~SSLClient(){
    io_service_.stop();
    join();
}

void Susi::SSLClient::send(std::string msg){
    io_service_.post(
        [this, msg]()
        {
          bool write_in_progress = !write_msgs_.empty();
          write_msgs_.push_back(msg);
          if (!write_in_progress) {
            do_write();
          }
        });
}

void Susi::SSLClient::do_connect(){
    boost::asio::async_connect(socket_->lowest_layer(), endpoint_iterator_,
        [this](boost::system::error_code ec, tcp::resolver::iterator) {
            if (!ec){
                socket_->async_handshake(boost::asio::ssl::stream_base::client,[this](boost::system::error_code ec){
                    if(!ec){
                        onConnect();
                        do_read();  
                    }else{
                        std::this_thread::sleep_for(std::chrono::seconds{1});
                        std::cout << "try reconnecting because of "<<ec.message()<<std::endl;
                        do_connect();
                    }
                });
            }else{
                std::this_thread::sleep_for(std::chrono::seconds{1});
                std::cout << "try reconnecting because of "<<ec.message()<<std::endl;
                do_connect();
            }
        });
}
void Susi::SSLClient::do_read(){
    socket_->async_read_some(boost::asio::buffer(buff, BUFFSIZE),
        [this](boost::system::error_code ec, std::size_t length) {
          if (!ec) {
            onData(buff,length);
            do_read();
          } else {
            onClose();
            socket_->lowest_layer().close();
            std::this_thread::sleep_for(std::chrono::seconds{1});
            std::cout << "try reconnecting"<<std::endl;    
            do_connect();
          }
    });
}
void Susi::SSLClient::do_write(){
    boost::asio::async_write(*socket_,
        boost::asio::buffer(write_msgs_.front().data(), write_msgs_.front().length()),
        [this](boost::system::error_code ec, std::size_t /*length*/) {
          if (!ec) {
            write_msgs_.pop_front();
            if (!write_msgs_.empty()) {
              do_write();
            }
          } else {
            onClose();
            socket_->lowest_layer().close();
            std::this_thread::sleep_for(std::chrono::seconds{1});
            std::cout << "try reconnecting"<<std::endl;    
            do_connect();
          }
        });
}
