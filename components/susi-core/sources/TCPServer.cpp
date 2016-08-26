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

#include "susi/TCPServer.h"

Susi::TCPServer::TCPServer(short port) : acceptor{io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v6(), port)} {
    do_accept();
    runloop = std::move(std::thread{[this]() {
        io_service.run();
    }});
}

void Susi::TCPServer::join() {
    if (runloop.joinable())runloop.join();
}

Susi::TCPServer::~TCPServer() {
    io_service.stop();
    join();
}

std::string Susi::TCPServer::getPeerCertificateHash(int id) {
    return "";
}

std::string Susi::TCPServer::getPeerCertificate(int id) {
    return "";
}

void Susi::TCPServer::send(int id, const char *data, size_t len) {
    auto session = sessions[id];
    if (!session) {
        throw std::runtime_error{"no such session"};
    }
    session->do_write(data, len);
}

void Susi::TCPServer::do_accept() {
    acceptor.async_accept(socket, [this](boost::system::error_code ec) {
        if (!ec) {
            //handle socket session
            int id = socket.native_handle();
            auto session = std::make_shared<Session>(std::move(socket), this);
            session->start();
            sessions[id] = std::move(session);
            onConnect(id);
        }
        do_accept();
    });
}

void Susi::TCPServer::Session::start() {
    do_read();
}

void Susi::TCPServer::Session::do_read() {
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
    [this, self](boost::system::error_code ec, std::size_t length) {
        if (ec) {
            server->onClose(socket_.native_handle());
            server->sessions.erase(socket_.native_handle());
        } else {
            server->onData(socket_.native_handle(), data_, length);
            do_read();
        }
    });
}

void Susi::TCPServer::Session::do_write(const char *data, size_t len) {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(data, len),
    [this, self](boost::system::error_code ec, std::size_t /*length*/) {
        if (ec) {
            server->onClose(socket_.native_handle());
            server->sessions.erase(socket_.native_handle());
        }
    });
}
