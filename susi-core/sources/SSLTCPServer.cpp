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

using X509_ptr = std::unique_ptr<X509, decltype(&::X509_free)>;
using BIO_MEM_ptr = std::unique_ptr<BIO, decltype(&::BIO_free)>;


Susi::SSLTCPServer::SSLTCPServer(short port, std::string keyFile, std::string certFile) : 
  keyFile{keyFile},
  certFile{certFile}, 
  acceptor{io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v6(), port)} {
        context.set_options(
            boost::asio::ssl::context::default_workarounds
            | boost::asio::ssl::context::no_sslv2
            | boost::asio::ssl::context::single_dh_use);
        context.set_verify_mode(boost::asio::ssl::verify_peer);
        context.set_verify_callback([](bool preverified, boost::asio::ssl::verify_context& ctx){return true;});
        context.use_private_key_file(keyFile, boost::asio::ssl::context::pem);
        context.use_certificate_chain_file(certFile);
    do_accept();
    runloop = std::move(std::thread{[this](){
        io_service.run();
    }});
}

void Susi::SSLTCPServer::join(){
    if(runloop.joinable())runloop.join();
}

Susi::SSLTCPServer::~SSLTCPServer(){
    io_service.stop();
    join();
}

std::string Susi::SSLTCPServer::getPeerCertificateHash(int id){
    try{
        return sessions.at(id)->getPeerCertificateHash();
    }catch(...){
        return "";
    }
}

std::string Susi::SSLTCPServer::getPeerCertificate(int id){
    try{
        return sessions.at(id)->getPeerCertificate();
    }catch(...){
        return "";
    }
}

void Susi::SSLTCPServer::send(int id, const char *data, size_t len){
    try{
        auto & session = sessions.at(id);
        session->do_write(data,len);
    }catch(...){}
}


void Susi::SSLTCPServer::do_accept(){
    auto session = std::make_shared<Session>(io_service,context,this);
    acceptor.async_accept(session->socket(), [this,session](boost::system::error_code ec) {
      if (!ec) {
        //handle socket session
        int id = session->socket().native_handle();
        session->start();
        sessions[id] = std::move(session);
        onConnect(id);
      }
      do_accept();
    });
}

ssl_socket::lowest_layer_type& Susi::SSLTCPServer::Session::socket() {
    return socket_.lowest_layer();
}

Susi::SSLTCPServer::Session::Session(boost::asio::io_service& io_service, boost::asio::ssl::context& context, SSLTCPServer * server) : 
  socket_{io_service, context}, 
  server{server} {}

void Susi::SSLTCPServer::Session::start() {
    socket_.async_handshake(boost::asio::ssl::stream_base::server,[this](boost::system::error_code ec){
        if(!ec){
            std::cout<<"handshake ready"<<std::endl;
            std::cout<<"peer cert: "<<getPeerCertificateHash()<<std::endl;
            do_read(); 
        }else{
            std::cerr<<ec.message()<<std::endl;
            server->onClose(socket().native_handle());
            socket().close();
        }
    });
}

void Susi::SSLTCPServer::Session::do_read() {
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        [this, self](boost::system::error_code ec, std::size_t length) {
            if(ec){
                server->onClose(socket().native_handle());
                server->sessions.erase(socket().native_handle());
            } else {
                server->onData(socket().native_handle(),data_,length);
                do_read();
            }
        });
}

void Susi::SSLTCPServer::Session::do_write(const char *data, size_t len) {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(data,len),
        [this, self](boost::system::error_code ec, std::size_t /*length*/) {
            if(ec){
                server->onClose(socket().native_handle());
                server->sessions.erase(socket().native_handle());
            }
        });
}

std::string Susi::SSLTCPServer::Session::getPeerCertificateHash(){
    X509* cert = SSL_get_peer_certificate(socket_.native_handle());
    #define SHA1LEN 20
    char buf[SHA1LEN];
    
    const EVP_MD *digest = EVP_sha1();
    unsigned int len = 0;
    
    int rc = X509_digest(cert, digest, (unsigned char*) buf, &len);
    std::cout<<"rc: "<<rc<<" len: "<<len<<std::endl;
    if (rc == 0 || len != SHA1LEN) {
        return "";
    }
    return base64_encode((const unsigned char*)buf, len);
}

std::string Susi::SSLTCPServer::Session::getPeerCertificate(){
    X509* cert = SSL_get_peer_certificate(socket_.native_handle());
    BIO_MEM_ptr bio(BIO_new(BIO_s_mem()), ::BIO_free);
    PEM_write_bio_X509(bio.get(), cert);
    BUF_MEM *mem = NULL;
    BIO_get_mem_ptr(bio.get(), &mem);
    std::string pem{mem->data, mem->length};
    return pem;
}
