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

#ifndef __ASIOTCPSERVER__
#define __ASIOTCPSERVER__

#include <memory>
#include <utility>
#include <thread>
#include <boost/asio.hpp>


namespace Susi {

class TCPServer {
  public:
    TCPServer(short port);

    void join();

    virtual ~TCPServer();

    virtual void onConnect(int id) = 0;
    virtual void onData(int id, char *data, size_t len) = 0;
    virtual void onClose(int id) = 0;

    std::string getPeerCertificateHash(int id);
    std::string getPeerCertificate(int id);


    void send(int id, const char *data, size_t len);

  protected:
    void do_accept();

    class Session : public std::enable_shared_from_this<Session> {
      public:
        Session(boost::asio::ip::tcp::socket socket, Susi::TCPServer *server) : socket_{std::move(socket)}, server{server} {}
        void start();
        void do_read();
        void do_write(const char *data, size_t len);
      protected:
        boost::asio::ip::tcp::socket socket_;
        Susi::TCPServer *server;
        enum { max_length = 1024 };
        char data_[max_length];
    };

    std::thread runloop;
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::acceptor acceptor{io_service};
    boost::asio::ip::tcp::socket socket{io_service};
    std::map<int, std::shared_ptr<Session>> sessions;
};

}

#endif // __ASIOTCPSERVER__
