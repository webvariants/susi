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

#include "susi/TCPClient.h"

Susi::TCPClient::TCPClient(std::string host, short port) {
    endpoint_iterator_ = resolver_.resolve({host, std::to_string(port)});
    do_connect();
    runloop_ = std::move(std::thread{[this]() {io_service_.run();}});
}

void Susi::TCPClient::join() {
    if (runloop_.joinable())runloop_.join();
}

Susi::TCPClient::~TCPClient() {
    io_service_.stop();
    join();
}

void Susi::TCPClient::send(std::string msg) {
    io_service_.post(
    [this, msg]() {
        bool write_in_progress = !write_msgs_.empty();
        write_msgs_.push_back(msg);
        if (!write_in_progress) {
            do_write();
        }
    });
}


void Susi::TCPClient::do_connect() {
    boost::asio::async_connect(socket_, endpoint_iterator_,
    [this](boost::system::error_code ec, tcp::resolver::iterator) {
        if (!ec) {
            onConnect();
            do_read();
        } else {
            std::this_thread::sleep_for(std::chrono::seconds{1});
            std::cout << "try reconnecting" << std::endl;
            do_connect();
        }
    });
}
void Susi::TCPClient::do_read() {
    socket_.async_read_some(boost::asio::buffer(buff, BUFFSIZE),
    [this](boost::system::error_code ec, std::size_t length) {
        if (!ec) {
            onData(buff, length);
            do_read();
        } else {
            onClose();
            socket_.close();
            std::this_thread::sleep_for(std::chrono::seconds{1});
            std::cout << "try reconnecting" << std::endl;
            do_connect();
        }
    });
}
void Susi::TCPClient::do_write() {
    boost::asio::async_write(socket_,
                             boost::asio::buffer(write_msgs_.front().data(), write_msgs_.front().length()),
    [this](boost::system::error_code ec, std::size_t /*length*/) {
        if (!ec) {
            std::cout << "wrote msg" << std::endl;
            write_msgs_.pop_front();
            if (!write_msgs_.empty()) {
                do_write();
            }
        } else {
            onClose();
            socket_.close();
            std::this_thread::sleep_for(std::chrono::seconds{1});
            std::cout << "try reconnecting" << std::endl;
            do_connect();
        }
    });
}
