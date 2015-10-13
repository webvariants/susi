/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 *
 * @author: Tino Rusch (tino.rusch@webvariants.de)
 */

#ifndef __DUKTAPEENGINE__
#define __DUKTAPEENGINE__

#include <sstream>
#include "susi/SusiClient.h"
#include "susi/duktape.h"

#include <boost/date_time/posix_time/posix_time.hpp>

namespace Susi {
namespace Duktape {

class JSEngine;

extern JSEngine *enginePtr;
extern std::string susiJS;

class JSEngine {
  public:
    JSEngine(Susi::SusiClient & susi, std::string script) :
        _client{susi},
        _script{script} {}

    // The start function subscribes to events and attaches its handlers
    void start();

    // When stopping, all subscriptions are deleted
    void stop();

  protected:
    boost::asio::io_service _ioservice;
    Susi::SusiClient & _client;
    std::string _script;

    duk_context *ctx;
    std::mutex mutex;
    boost::asio::deadline_timer _keepAliveTimeout{_ioservice, boost::posix_time::seconds(5)};
    std::thread _runloop;

    void _kickoffKeepAlive(){
      _keepAliveTimeout.async_wait([this](const boost::system::error_code&){
        this->_kickoffKeepAlive();
      });
    }

    enum {
        OUTPUT = 0,
        INPUT = 1
    };

    std::map<std::string, Susi::EventPtr> pendingEvents;
    std::map<std::string, long> registerIdsConsumer;
    std::map<std::string, long> registerIdsProcessor;

    static duk_ret_t js_registerConsumer(duk_context *ctx) ;
    static duk_ret_t js_registerProcessor(duk_context *ctx) ;
    static duk_ret_t js_publish(duk_context *ctx) ;
    static duk_ret_t js_ack(duk_context *ctx) ;
    static duk_ret_t js_dismiss(duk_context *ctx) ;
    static duk_ret_t js_log(duk_context *ctx) ;
    static duk_ret_t js_unregisterConsumer(duk_context *ctx) ;
    static duk_ret_t js_unregisterProcessor(duk_context *ctx) ;
    static duk_ret_t js_setTimeout(duk_context *ctx) ;

    void registerProcessor(std::string topic);
    void registerConsumer(std::string topic);
    void unregisterConsumer(std::string topic);
    void unregisterProcessor(std::string topic);
    void publish(std::string eventData);
    void ack(std::string eventData);
    void dismiss(std::string eventData);

    void init();

};

}
}

#endif // __DUKTAPEENGINE__
