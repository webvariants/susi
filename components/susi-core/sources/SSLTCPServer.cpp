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

#include "susi/SSLTCPServer.h"
#include "susi/base64.h"

using BIO_MEM_ptr = std::unique_ptr<BIO, decltype(&::BIO_free)>;


Susi::SSLTCPServer::SSLTCPServer(short port, std::string keyFile, std::string certFile) :
    keyFile{keyFile},
    certFile{certFile},
    acceptor{io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v6(), port)} {
    context.set_options(
        boost::asio::ssl::context::default_workarounds
        | boost::asio::ssl::context::no_sslv2
        | boost::asio::ssl::context::single_dh_use);
    context.set_verify_mode(boost::asio::ssl::verify_fail_if_no_peer_cert | boost::asio::ssl::verify_peer);
    context.set_verify_callback([this](bool preverified, boost::asio::ssl::verify_context & ctx) {
        return true;
    });
    context.use_private_key_file(keyFile, boost::asio::ssl::context::pem);
    context.use_certificate_chain_file(certFile);
    do_accept();
    runloop = std::move(std::thread{[this]() {
        io_service.run();
    }});
}

void Susi::SSLTCPServer::join() {
    if (runloop.joinable())runloop.join();
}

Susi::SSLTCPServer::~SSLTCPServer() {
    io_service.stop();
    join();
}

std::string Susi::SSLTCPServer::getPeerCertificateHash(int id) {
    try {
        return sessions.at(id)->getPeerCertificateHash();
    } catch (...) {
        return "0000000000000000000000000000000000000000000000000000000000000000";
    }
}

std::string Susi::SSLTCPServer::getPeerCertificate(int id) {
    try {
        return sessions.at(id)->getPeerCertificate();
    } catch (...) {
        return "";
    }
}

void Susi::SSLTCPServer::send(int id, const char *data, size_t len) {
    try {
        auto & session = sessions.at(id);
        session->send({data, len});
    } catch (const std::exception & e) {
        std::cout<<"error in send: "<<e.what()<<std::endl;

    }
}


void Susi::SSLTCPServer::do_accept() {
    auto session = std::make_shared<Session>(io_service, context, this);
    acceptor.async_accept(session->socket(), [this, session](boost::system::error_code ec) {
        if (!ec) {
            //handle socket session
            int id = session->socket().native_handle();
            session->start();
            sessions[id] = std::move(session);
        }
        do_accept();
    });
}

ssl_socket::lowest_layer_type& Susi::SSLTCPServer::Session::socket() {
    return socket_.lowest_layer();
}

Susi::SSLTCPServer::Session::Session(boost::asio::io_service& io_service, boost::asio::ssl::context& context, SSLTCPServer * server) :
    io_service_{io_service},
    socket_{io_service, context},
    server{server} {}

void Susi::SSLTCPServer::Session::start() {
    socket_.async_handshake(boost::asio::ssl::stream_base::server, [this](boost::system::error_code ec) {
        if (!ec) {
            server->onConnect(socket().native_handle());
            do_read();
        } else {
            std::cerr << ec.message() << std::endl;
            if(!this->onCloseCalled){
                server->onClose(socket().native_handle());
                this->onCloseCalled = true;
                server->sessions.erase(socket().native_handle());
            }
        }
    });
}

void Susi::SSLTCPServer::Session::do_read() {
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
    [this, self](boost::system::error_code ec, std::size_t length) {
        if (ec) {
            if(!this->onCloseCalled){
                server->onClose(socket().native_handle());
                this->onCloseCalled = true;
                server->sessions.erase(socket().native_handle());
            }
        } else {
            server->onData(socket().native_handle(), data_, length);
            do_read();
        }
    });
}

void Susi::SSLTCPServer::Session::do_write() {
    auto self(shared_from_this());
    boost::asio::async_write(socket_,
                             boost::asio::buffer(write_msgs_.front().data(), write_msgs_.front().length()),
    [this, self](boost::system::error_code ec, std::size_t /*length*/) {
        if (!ec) {
            write_msgs_.pop_front();
            if (!write_msgs_.empty()) {
                do_write();
            }
        } else {
            if(!this->onCloseCalled){
                server->onClose(socket().native_handle());
                this->onCloseCalled = true;
                server->sessions.erase(socket().native_handle());
            }
        }
    });
}

void Susi::SSLTCPServer::Session::send(std::string msg) {
    io_service_.post(
    [this, msg]() {
        bool write_in_progress = !write_msgs_.empty();
        write_msgs_.push_back(msg);
        if (!write_in_progress) {
            do_write();
        }
    });
}

std::string Susi::SSLTCPServer::Session::getPeerCertificateHash() {
    std::string cert = getPeerCertificate();
    SHA3 sha3;
    return sha3(cert);
}

std::string Susi::SSLTCPServer::Session::getPeerCertificate() {
    X509* cert = SSL_get_peer_certificate(socket_.native_handle());
    BIO_MEM_ptr bio(BIO_new(BIO_s_mem()), ::BIO_free);
    PEM_write_bio_X509(bio.get(), cert);
    BUF_MEM *mem = NULL;
    BIO_get_mem_ptr(bio.get(), &mem);
    std::string pem{mem->data, mem->length};
    return pem;
}
