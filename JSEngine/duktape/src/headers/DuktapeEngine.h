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


#include "world/BaseComponent.h"
#include <sstream>
#include "duktape.h"

namespace Duktape {

class JSEngine;

extern JSEngine *enginePtr;

class JSEngine : public Susi::System::BaseComponent {
public:
	JSEngine(Susi::System::ComponentManager *mgr) : Susi::System::BaseComponent{mgr} {}

	// The start function subscribes to events and attaches its handlers
	virtual void start() override {
		enginePtr = this;
		ctx = duk_create_heap_default();
		if (!ctx) {
	        printf("Failed to create a Duktape heap.\n");
	        exit(1);
	    }
	    duk_push_global_object(ctx);
	    duk_push_c_function(ctx, js_registerConsumer, 1 /*nargs*/);
	    duk_put_prop_string(ctx, -2, "_registerConsumer");
	    duk_push_c_function(ctx, js_registerProcessor, 1 /*nargs*/);
	    duk_put_prop_string(ctx, -2, "_registerProcessor");
	    duk_push_c_function(ctx, js_publish, 1 /*nargs*/);
	    duk_put_prop_string(ctx, -2, "_publish");
	    duk_push_c_function(ctx, js_ack, 1 /*nargs*/);
	    duk_put_prop_string(ctx, -2, "_ack");
	    if (duk_peval_file(ctx, "susi.js") != 0) {
	        printf("Error: %s\n", duk_safe_to_string(ctx, -1));
	        exit(1);
	    }
	}

	// When stopping, all subscriptions are deleted
	virtual void stop() override {
		unsubscribeAll();
	}

protected:
	duk_context *ctx;
	std::mutex mutex;

	std::map<std::string,Susi::Events::EventPtr> pendingEvents;

	static duk_ret_t js_registerConsumer(duk_context *ctx) {
	    size_t sz;
	    const char *val = duk_require_lstring(ctx, 0, &sz);
	    enginePtr->registerConsumer(val);
	    duk_push_true(ctx);
	    return 1;
	}

	static duk_ret_t js_registerProcessor(duk_context *ctx) {
	    size_t sz;
	    const char *val = duk_require_lstring(ctx, 0, &sz);
	    enginePtr->registerProcessor(val);
	    duk_push_true(ctx);
	    return 1;
	}

	static duk_ret_t js_publish(duk_context *ctx) {
	    size_t sz;
	    const char *val = duk_require_lstring(ctx, 0, &sz);
	    enginePtr->publish(val);
	    duk_push_true(ctx);
	    return 1;
	}

	static duk_ret_t js_ack(duk_context *ctx) {
	    size_t sz;
	    const char *val = duk_require_lstring(ctx, 0, &sz);
	    enginePtr->ack(val);
	    duk_push_true(ctx);
	    return 1;
	}

	void registerProcessor(std::string topic){
		BaseComponent::subscribe(topic,[this](Susi::Events::EventPtr event){
			std::lock_guard<std::mutex> lock{mutex};
			auto eventString = event->toString();
			
			pendingEvents[event->id] = std::move(event);
			duk_push_global_object(ctx);
            duk_get_prop_string(ctx, -1 /*index*/, "_processProcessorEvent");
            duk_push_string(ctx, eventString.c_str());
            if (duk_pcall(ctx, 1 /*nargs*/) != 0) {
                printf("Error: %s\n", duk_safe_to_string(ctx, -1));
            }
            duk_pop(ctx);  /* pop result/error */
		});
	}

	void registerConsumer(std::string topic){
		Susi::Events::Consumer consumer = [this](Susi::Events::SharedEventPtr event){
			std::lock_guard<std::mutex> lock{mutex};
			duk_push_global_object(ctx);
            duk_get_prop_string(ctx, -1 /*index*/, "_processConsumerEvent");
            duk_push_string(ctx, event->toString().c_str());
            if (duk_pcall(ctx, 1 /*nargs*/) != 0) {
                printf("Error: %s\n", duk_safe_to_string(ctx, -1));
            }
            duk_pop(ctx);  /* pop result/error */
		};
		BaseComponent::subscribe(topic,consumer);
	}

	void publish(std::string eventData){
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
                printf("Error: %s\n", duk_safe_to_string(ctx, -1));
            }
            duk_pop(ctx);  /* pop result/error */	
		});
	}

	void ack(std::string eventData){
		auto any = Susi::Util::Any::fromJSONString(eventData);
		Susi::Events::Event rawEvent{any};
		*pendingEvents[rawEvent.id] = rawEvent;
		pendingEvents.erase(rawEvent.id);
	}
};


}

#endif // __DUKTAPEENGINE__
