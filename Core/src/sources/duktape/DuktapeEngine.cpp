#include "duktape/DuktapeEngine.h"

Susi::Duktape::JSEngine *Susi::Duktape::enginePtr;

void Susi::Duktape::JSEngine::start() {
	Susi::Duktape::enginePtr = this;
	
    init();

    if(sourceFile != ""){
        if (duk_peval_file(ctx, sourceFile.c_str()) != 0) {
            LOG(ERROR) << "processing usersource: " << duk_safe_to_string(ctx, -1);
        }
    }

    LOG(DEBUG) << "started Susi::duktape engine and loaded "<<sourceFile;
}

void Susi::Duktape::JSEngine::init(){
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
    duk_push_c_function(ctx, js_unregister, 1 /*nargs*/);
    duk_put_prop_string(ctx, -2, "_unregister");
    duk_push_c_function(ctx, js_publish, 1 /*nargs*/);
    duk_put_prop_string(ctx, -2, "_publish");
    duk_push_c_function(ctx, js_ack, 1 /*nargs*/);
    duk_put_prop_string(ctx, -2, "_ack");
    duk_push_c_function(ctx, js_log, 1 /*nargs*/);
    duk_put_prop_string(ctx, -2, "log");

    if(duk_peval_lstring(ctx, Susi::Duktape::susiJS.c_str(), Susi::Duktape::susiJS.size()) != 0){
        LOG(ERROR) << "processing susiJS: " << duk_safe_to_string(ctx, -1);
    }
}

void Susi::Duktape::JSEngine::stop(){
	unsubscribeAll();
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
    try{
    	enginePtr->publish(val);
    }catch(const std::exception & e){
    	LOG(ERROR) << "publish failed: "<<e.what();
    	duk_push_false(ctx);
    	return 1;
    }
    duk_push_true(ctx);
    return 1;
}

duk_ret_t Susi::Duktape::JSEngine::js_ack(duk_context *ctx) {
    size_t sz;
    const char *val = duk_require_lstring(ctx, 0, &sz);
    if(val!=nullptr){
	    enginePtr->ack(val);
	    duk_push_true(ctx);
    }else{
    	duk_push_false(ctx);
    }
    return 1;
}

duk_ret_t Susi::Duktape::JSEngine::js_log(duk_context *ctx) {
    size_t sz;
    const char *val = duk_require_lstring(ctx, 0, &sz);
    LOG(INFO)<<val;
    duk_push_true(ctx);
    return 1;
}

duk_ret_t Susi::Duktape::JSEngine::js_unregister(duk_context *ctx) {
    size_t sz;
    const char *topic = duk_require_lstring(ctx, 0, &sz);
    enginePtr->unregister(topic);
    duk_push_true(ctx);
    return 1;
}

void Susi::Duktape::JSEngine::registerProcessor(std::string topic){
	registerIds[topic] = BaseComponent::subscribe(topic,[this,topic](Susi::Events::EventPtr event){
		std::lock_guard<std::mutex> lock{mutex};
		auto eventString = event->toString();
		pendingEvents[event->id] = std::move(event);
		duk_push_global_object(ctx);
        duk_get_prop_string(ctx, -1 /*index*/, "_processProcessorEvent");
        duk_push_string(ctx, eventString.c_str());
        duk_push_string(ctx, topic.c_str());
        if (duk_pcall(ctx, 2 /*nargs*/) != 0) {
            LOG(ERROR) << (std::string{"Error: "}+duk_safe_to_string(ctx, -1));
        }
        duk_pop(ctx);  /* pop result/error */
	});
}

void Susi::Duktape::JSEngine::unregister(std::string topic){
    BaseComponent::unsubscribe(registerIds[topic]);
    registerIds.erase(topic);
}

void Susi::Duktape::JSEngine::registerConsumer(std::string topic){
	Susi::Events::Consumer consumer = [this,topic](Susi::Events::SharedEventPtr event){
		std::lock_guard<std::mutex> lock{mutex};
		duk_push_global_object(ctx);
        duk_get_prop_string(ctx, -1 /*index*/, "_processConsumerEvent");
        duk_push_string(ctx, event->toString().c_str());
        duk_push_string(ctx, topic.c_str());
        if (duk_pcall(ctx, 2 /*nargs*/) != 0) {
            LOG(ERROR) << (std::string{"Error: "}+duk_safe_to_string(ctx, -1));
        }
        duk_pop(ctx);  /* pop result/error */
	};
	registerIds[topic] = BaseComponent::subscribe(topic,consumer);
}

void Susi::Duktape::JSEngine::publish(std::string eventData){
    LOG(DEBUG) << eventData;
	auto any = Susi::Util::Any::fromJSONString(eventData);
	Susi::Events::Event rawEvent{any};
	auto event = createEvent(rawEvent.topic);
	*event = rawEvent;
	BaseComponent::publish(std::move(event),[this](Susi::Events::SharedEventPtr event){
		std::lock_guard<std::mutex> lock{mutex};
		duk_push_global_object(ctx);
        duk_get_prop_string(ctx, -1 /*index*/, "_processAck");
        duk_push_string(ctx, event->toString().c_str());
        if (duk_pcall(ctx, 1 /*nargs*/) != 0) {
            LOG(ERROR) << (std::string{"Error: "}+duk_safe_to_string(ctx, -1));
        }
        duk_pop(ctx);  /* pop result/error */	
	});
}

void Susi::Duktape::JSEngine::ack(std::string eventData){
	auto any = Susi::Util::Any::fromJSONString(eventData);
	Susi::Events::Event rawEvent{any};
	try{
		auto & pendingEvent = pendingEvents.at(rawEvent.id);
		*pendingEvent = rawEvent;
		pendingEvents.erase(rawEvent.id);
	}catch(...){}
}

