/*
#include "webstack/HttpServerLibEvent.h"

Susi::Webstack::HttpServer::HttpServer(std::string _srvAddress, std::uint16_t _srvPort, int _srvThreadcount) {
    srvAddress = _srvAddress;
    srvPort = _srvPort;
    srvThreadcount = _srvThreadcount;

    auto ThreadFunc = [&] () {
        try {

        }
        catch () {

        }
    };
}

void Susi::Webstack::HttpServer::onRequest(evhttp_request *req, void*) {
    auto *outBuf = evhttp_request_get_output_buffer(req);
    if(!outBuf) {
        return;
    }
    evbuffer_add_printf(outBuf, "<html><body><h1>Hello World!</h1></body></html>");
    evhttp_send_reply(req, HTTP_OK, "", outBuf);
}

Susi::Webstack::HttpServer::~HttpServer() {
}

*/
