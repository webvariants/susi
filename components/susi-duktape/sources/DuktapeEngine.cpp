#include "susi/DuktapeEngine.h"

Susi::Duktape::JSEngine *Susi::Duktape::enginePtr;

void Susi::Duktape::JSEngine::start() {
    Susi::Duktape::enginePtr = this;

    //keep the asio event loop running;
    _kickoffKeepAlive();
    _runloop = std::move(std::thread{[this](){
      _ioservice.run();
    }});

    init();

    if (_script != "") {
        std::lock_guard<std::mutex> lock{enginePtr->mutex};
        if (duk_peval_file(ctx, _script.c_str()) != 0) {
            std::cerr << "processing usersource: " << duk_safe_to_string(ctx, -1) << std::endl;
        }
    }

    std::cout << "started Susi::duktape engine and loaded " << _script <<std::endl;
}

void Susi::Duktape::JSEngine::init() {
    ctx = duk_create_heap_default();
    if (!ctx) {
        printf("Failed to create a Susi::Duktape heap.\n");
        exit(1);
    }
    duk_push_global_object(ctx);
    duk_push_c_function(ctx, js_registerConsumer, 1 /*nargs*/);
    duk_put_prop_string(ctx, -2, "_registerConsumer");
    duk_push_c_function(ctx, js_registerProcessor, 1 /*nargs*/);
    duk_put_prop_string(ctx, -2, "_registerProcessor");
    duk_push_c_function(ctx, js_unregisterProcessor, 1 /*nargs*/);
    duk_put_prop_string(ctx, -2, "_unregisterProcessor");
    duk_push_c_function(ctx, js_unregisterConsumer, 1 /*nargs*/);
    duk_put_prop_string(ctx, -2, "_unregisterConsumer");
    duk_push_c_function(ctx, js_publish, 1 /*nargs*/);
    duk_put_prop_string(ctx, -2, "_publish");
    duk_push_c_function(ctx, js_ack, 1 /*nargs*/);
    duk_put_prop_string(ctx, -2, "_ack");
    duk_push_c_function(ctx, js_dismiss, 1 /*nargs*/);
    duk_put_prop_string(ctx, -2, "_dismiss");
    duk_push_c_function(ctx, js_log, 1 /*nargs*/);
    duk_put_prop_string(ctx, -2, "log");
    duk_push_c_function(ctx, js_setTimeout, 1 /*nargs*/);
    duk_put_prop_string(ctx, -2, "_setTimeout");

    if (duk_peval_lstring(ctx, Susi::Duktape::susiJS.c_str(), Susi::Duktape::susiJS.size()) != 0) {
        std::cerr << "processing susiJS: " << duk_safe_to_string(ctx, -1) << std::endl;
    }
}

void Susi::Duktape::JSEngine::stop() {
    //unsubscribeAll();
}

duk_ret_t Susi::Duktape::JSEngine::js_registerConsumer(duk_context *ctx) {
    size_t sz;
    const char *val = duk_require_lstring(ctx, 0, &sz);
    enginePtr->registerConsumer(val);
    duk_push_true(ctx);
    return 1;
}

duk_ret_t Susi::Duktape::JSEngine::js_registerProcessor(duk_context *ctx) {
    size_t sz;
    const char *val = duk_require_lstring(ctx, 0, &sz);
    enginePtr->registerProcessor(val);
    duk_push_true(ctx);
    return 1;
}

duk_ret_t Susi::Duktape::JSEngine::js_publish(duk_context *ctx) {
    size_t sz;
    const char *val = duk_require_lstring(ctx, 0, &sz);
    try {
        enginePtr->publish(val);
    } catch (const std::exception & e) {
        std::cerr << "publish failed: " << e.what() << std::endl;
        duk_push_false(ctx);
        return 1;
    }
    duk_push_true(ctx);
    return 1;
}

duk_ret_t Susi::Duktape::JSEngine::js_ack(duk_context *ctx) {
    size_t sz;
    const char *val = duk_require_lstring(ctx, 0, &sz);
    if (val != nullptr) {
        enginePtr->ack(val);
        duk_push_true(ctx);
    } else {
        duk_push_false(ctx);
    }
    return 1;
}

duk_ret_t Susi::Duktape::JSEngine::js_dismiss(duk_context *ctx) {
    size_t sz;
    const char *val = duk_require_lstring(ctx, 0, &sz);
    if (val != nullptr) {
        enginePtr->dismiss(val);
        duk_push_true(ctx);
    } else {
        duk_push_false(ctx);
    }
    return 1;
}

duk_ret_t Susi::Duktape::JSEngine::js_log(duk_context *ctx) {
    size_t sz;
    const char *val = duk_require_lstring(ctx, 0, &sz);
    std::cout << val << std::endl;
    duk_push_true(ctx);
    return 1;
}

duk_ret_t Susi::Duktape::JSEngine::js_setTimeout(duk_context *ctx) {
    int arg = duk_require_int(ctx, 0 /*index*/);
    auto timeout = std::make_shared<boost::asio::deadline_timer>(enginePtr->_ioservice, boost::posix_time::milliseconds(arg));
    timeout->async_wait([timeout,ctx](const boost::system::error_code&){
        std::lock_guard<std::mutex> lock{enginePtr->mutex};
        duk_push_global_object(ctx);
        duk_get_prop_string(ctx, -1 /*index*/, "_checkTimeouts");
        if (duk_pcall(ctx, 0 /*nargs*/) != 0) {
            std::string err = duk_safe_to_string(ctx, -1);
            std::cerr << err << std::endl;
            throw std::runtime_error{err};
        }
        duk_pop(ctx);  /* pop result/error */
    });
    return 1;
}



duk_ret_t Susi::Duktape::JSEngine::js_unregisterConsumer(duk_context *ctx) {
    size_t sz;
    const char *topic = duk_require_lstring(ctx, 0, &sz);
    enginePtr->unregisterConsumer(topic);
    duk_push_true(ctx);
    return 1;
}

duk_ret_t Susi::Duktape::JSEngine::js_unregisterProcessor(duk_context *ctx) {
    size_t sz;
    const char *topic = duk_require_lstring(ctx, 0, &sz);
    enginePtr->unregisterProcessor(topic);
    duk_push_true(ctx);
    return 1;
}

void Susi::Duktape::JSEngine::registerProcessor(std::string topic) {
    registerIdsProcessor[topic] = _client.registerProcessor(topic, [this, topic](Susi::EventPtr event) {
        const std::string eventID = event->id;
        try {
            std::lock_guard<std::mutex> lock{mutex};
            auto eventString = event->toString();
            pendingEvents[eventID] = std::move(event);
            duk_push_global_object(ctx);
            duk_get_prop_string(ctx, -1 /*index*/, "_processProcessorEvent");
            duk_push_string(ctx, eventString.c_str());
            duk_push_string(ctx, topic.c_str());
            if (duk_pcall(ctx, 2 /*nargs*/) != 0) {
                std::string err = duk_safe_to_string(ctx, -1);
                std::cerr << err << std::endl;
                throw std::runtime_error{err};
            }
            duk_pop(ctx);  /* pop result/error */
        } catch (const std::exception & e) {
            std::string err = e.what();
            auto & pendingEvent = pendingEvents.at(eventID);
            pendingEvent->headers.push_back({"Error", err});
            pendingEvents.erase(eventID);
        }
    });
}

void Susi::Duktape::JSEngine::unregisterProcessor(std::string topic) {
    _client.unregisterProcessor(registerIdsProcessor[topic]);
    registerIdsProcessor.erase(topic);
}

void Susi::Duktape::JSEngine::unregisterConsumer(std::string topic) {
    _client.unregisterConsumer(registerIdsConsumer[topic]);
    registerIdsConsumer.erase(topic);
}

void Susi::Duktape::JSEngine::registerConsumer(std::string topic) {
    Susi::Consumer consumer = [this, topic](Susi::SharedEventPtr event) {
        std::lock_guard<std::mutex> lock{mutex};
        duk_push_global_object(ctx);
        duk_get_prop_string(ctx, -1 /*index*/, "_processConsumerEvent");
        duk_push_string(ctx, event->toString().c_str());
        duk_push_string(ctx, topic.c_str());
        if (duk_pcall(ctx, 2 /*nargs*/) != 0) {
            std::cerr << (std::string{"Error: "} +duk_safe_to_string(ctx, -1)) << std::endl;
        }
        duk_pop(ctx);  /* pop result/error */
    };
    registerIdsConsumer[topic] = _client.registerConsumer(topic, consumer);
}

void Susi::Duktape::JSEngine::publish(std::string eventData) {
    auto any = BSON::Value::fromJSON(eventData);
    Susi::Event rawEvent{any};
    auto event = _client.createEvent(rawEvent.topic);
    *event = rawEvent;
    _client.publish(std::move(event), [this](Susi::SharedEventPtr event) {
        std::lock_guard<std::mutex> lock{mutex};
        duk_push_global_object(ctx);
        duk_get_prop_string(ctx, -1 /*index*/, "_processAck");
        duk_push_string(ctx, event->toString().c_str());
        if (duk_pcall(ctx, 1 /*nargs*/) != 0) {
            std::cerr << (std::string{"Error: "} +duk_safe_to_string(ctx, -1)) << std::endl;
        }
        duk_pop(ctx);  /* pop result/error */
    });
}

void Susi::Duktape::JSEngine::ack(std::string eventData) {
    auto any = BSON::Value::fromJSON(eventData);
    Susi::Event rawEvent{any};
    try {
        auto & pendingEvent = pendingEvents.at(rawEvent.id);
        *pendingEvent = rawEvent;
        _client.ack(std::move(pendingEvent));
        pendingEvents.erase(rawEvent.id);
    } catch (...) {}
}

void Susi::Duktape::JSEngine::dismiss(std::string eventData) {
    auto any = BSON::Value::fromJSON(eventData);
    Susi::Event rawEvent{any};
    try {
        auto & pendingEvent = pendingEvents.at(rawEvent.id);
        *pendingEvent = rawEvent;
        _client.dismiss(std::move(pendingEvent));
        pendingEvents.erase(rawEvent.id);
    } catch (...) {}
}
