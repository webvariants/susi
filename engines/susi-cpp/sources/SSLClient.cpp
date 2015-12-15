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

Susi::SSLClient::SSLClient(std::string host, short port) {
    try{
        endpoint_iterator_ = resolver_.resolve({host, std::to_string(port)});
    }catch(const std::exception & e){
        std::cerr<<"error while resolving the given endpoint"<<std::endl;
        return;
    }
    do_resolve(host,std::to_string(port));
    runloop_ = std::move(std::thread{[this]() {io_service_.run(); std::cout<<"io_service returned..."<<std::endl;}});
}

Susi::SSLClient::SSLClient(std::string host, short port, std::string keyfile, std::string certfile) {
    context_.use_private_key_file(keyfile, boost::asio::ssl::context::pem);
    context_.use_certificate_file(certfile, boost::asio::ssl::context::pem);
    socket_ = std::make_shared<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>(io_service_, context_);
    endpoint_iterator_ = resolver_.resolve({host, std::to_string(port)});
    do_connect();
    runloop_ = std::move(std::thread{[this]() {io_service_.run();}});
}

void Susi::SSLClient::join() {
    if (runloop_.joinable())runloop_.join();
}

Susi::SSLClient::~SSLClient() {
    io_service_.stop();
    join();
}

void Susi::SSLClient::send(std::string msg) {
    io_service_.post(
    [this, msg]() {
        bool write_in_progress = !write_msgs_.empty();
        write_msgs_.push_back(msg);
        if (!write_in_progress) {
            do_write();
        }
    });
}

void Susi::SSLClient::do_resolve(std::string host, std::string port) {
    resolver_.async_resolve({host, port},[this,host,port](boost::system::error_code ec,  boost::asio::ip::tcp::resolver::iterator iterator){
        if(!ec){
            endpoint_iterator_ = iterator;
            do_connect();
        }else{
            std::this_thread::sleep_for(std::chrono::seconds{1});
            std::cout << "try reconnecting because of " << ec.message() << std::endl;
            do_resolve(host,port);
        }
    });
}

void Susi::SSLClient::do_connect() {
    boost::asio::async_connect(socket_->lowest_layer(), endpoint_iterator_,
    [this](boost::system::error_code ec, tcp::resolver::iterator) {
        if (!ec) {
            socket_->async_handshake(boost::asio::ssl::stream_base::client, [this](boost::system::error_code ec) {
                if (!ec) {
                    onConnect();
                    do_read();
                } else {
                    std::this_thread::sleep_for(std::chrono::seconds{1});
                    std::cout << "try reconnecting because of " << ec.message() << std::endl;
                    do_connect();
                }
            });
        } else {
            std::this_thread::sleep_for(std::chrono::seconds{1});
            std::cout << "try reconnecting because of " << ec.message() << std::endl;
            do_connect();
        }
    });
}
void Susi::SSLClient::do_read() {
    socket_->async_read_some(boost::asio::buffer(buff, BUFFSIZE),
    [this](boost::system::error_code ec, std::size_t length) {
        if (!ec) {
            onData(buff, length);
            do_read();
        } else {
            onClose();
            socket_->lowest_layer().close();
            std::this_thread::sleep_for(std::chrono::seconds{1});
            std::cout << "try reconnecting" << std::endl;
            do_connect();
        }
    });
}
void Susi::SSLClient::do_write() {
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
            std::cout << "try reconnecting" << std::endl;
            do_connect();
        }
    });
}

std::string Susi::SSLClient::getCertificateHash() {
    std::string cert = getCertificate();
    SHA3 sha3;
    return sha3(cert);
}

std::string Susi::SSLClient::getCertificate() {
    using BIO_MEM_ptr = std::unique_ptr<BIO, decltype(&::BIO_free)>;
    X509* cert = SSL_get_certificate(socket_->native_handle());
    BIO_MEM_ptr bio(BIO_new(BIO_s_mem()), ::BIO_free);
    PEM_write_bio_X509(bio.get(), cert);
    BUF_MEM *mem = NULL;
    BIO_get_mem_ptr(bio.get(), &mem);
    std::string pem{mem->data, mem->length};
    return pem;
}
