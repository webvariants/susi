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

#ifndef __TCPCLIENT__
#define __TCPCLIENT__

#include <iostream>
#include <thread>
#include <deque>
#include <boost/asio.hpp>
#include <chrono>

using boost::asio::ip::tcp;

namespace Susi {
class TCPClient {
  public:
    TCPClient(std::string host, short port);

    void join();

    virtual ~TCPClient();

    virtual void onConnect() = 0;
    virtual void onData(char *data, size_t len) = 0;
    virtual void onClose() = 0;

    void send(std::string msg);

  protected:

    void do_connect();
    void do_read();
    void do_write();

    boost::asio::io_service io_service_;
    tcp::resolver::iterator endpoint_iterator_;
    tcp::resolver resolver_{io_service_};
    tcp::socket socket_{io_service_};
    enum {BUFFSIZE = 1024};
    char buff[BUFFSIZE];
    std::deque<std::string> write_msgs_;
    std::thread runloop_;
};

}

#endif // __TCPCLIENT__
