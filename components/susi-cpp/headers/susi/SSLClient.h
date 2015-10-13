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

#ifndef __SSLCLIENT__
#define __SSLCLIENT__

#include <iostream>
#include <thread>
#include <deque>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <chrono>

#include "susi/sha3.h"

using boost::asio::ip::tcp;

namespace Susi {
class SSLClient {
  public:
    SSLClient(std::string host, short port);

    SSLClient(std::string host, short port, std::string keyfile, std::string certfile);

    void join();

    virtual ~SSLClient();

    virtual void onConnect() = 0;
    virtual void onData(char *data, size_t len) = 0;
    virtual void onClose() = 0;

    void send(std::string msg);

    std::string getCertificateHash();
    std::string getCertificate();


  protected:

    void do_connect();
    void do_read();
    void do_write();

    boost::asio::io_service io_service_;
    tcp::resolver::iterator endpoint_iterator_;
    tcp::resolver resolver_{io_service_};
    boost::asio::ssl::context context_{boost::asio::ssl::context::tlsv12};
    std::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> socket_;
    enum {BUFFSIZE = 1024};
    char buff[BUFFSIZE];
    std::deque<std::string> write_msgs_;
    std::thread runloop_;
};

}

#endif // __SSLCLIENT__
