/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 *
 * based on the libevent http-server sample.
 *
 * @author: Christian Sonderfeld (christian.sonderfeld@webvariants.de)
 */
/*

#ifndef __LIBEVENTHTTPSERVER__
#define __LIBEVENTHTTPSERVER__

#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/keyvalq_struct.h>
#include <memory>
#include <chrono>

namespace Susi {
namespace Webstack {

class HttpServer {
protected:
    std::string srvAddress = "127.0.0.1";
    std::uint16_t srvPort = 5555;
    int srvThreadcount = 4;
    void onRequest(evhttp_request *req, void *);
public:
    HttpServer(std::string const &address,
               std::uint16_t port,
               std::uint16_t threadcount,
               OnrequestFunc o);
    ~HttpServer();
};

}
}


#endif // __LIBEVENTHTTPSERVER__
*/
