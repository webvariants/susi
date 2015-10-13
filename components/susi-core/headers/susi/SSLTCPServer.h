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

#ifndef __ASIOSSLTCPSERVER__
#define __ASIOSSLTCPSERVER__

#include <memory>
#include <utility>
#include <deque>
#include <thread>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include "susi/sha3.h"


typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket;

namespace Susi {

class SSLTCPServer {
  public:
    SSLTCPServer(short port, std::string keyFile, std::string certFile);

    void join();

    virtual ~SSLTCPServer();

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
        boost::asio::io_service& io_service_;
        ssl_socket socket_;
        Susi::SSLTCPServer *server;
        bool onCloseCalled = false;

        ssl_socket::lowest_layer_type& socket();

        Session(boost::asio::io_service& io_service, boost::asio::ssl::context& context, SSLTCPServer * server);

        void start();

        void do_read();

        void do_write();

        void send(std::string msg);

        std::string getPeerCertificateHash();
        std::string getPeerCertificate();

        std::string keyFile;
        std::string certFile;
        enum { max_length = 1024 };
        char data_[max_length];
        std::deque<std::string> write_msgs_;
    };


    std::string keyFile;
    std::string certFile;
    std::thread runloop;
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::acceptor acceptor{io_service};
    boost::asio::ssl::context context{boost::asio::ssl::context::tlsv12_server};
    std::map<int, std::shared_ptr<Session>> sessions;
};

}

#endif // __ASIOSSLTCPSERVER__
