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
extern std::string susiJS;

class JSEngine : public Susi::System::BaseComponent {
public:
	JSEngine(Susi::System::ComponentManager *mgr,std::string src) : 
		Susi::System::BaseComponent{mgr},
		sourceFile{src} {}

	// The start function subscribes to events and attaches its handlers
	virtual void start() override;

	// When stopping, all subscriptions are deleted
	virtual void stop() override;

protected:
	std::string sourceFile;
	duk_context *ctx;
	std::mutex mutex;

	std::map<std::string,Susi::Events::EventPtr> pendingEvents;

	static duk_ret_t js_registerConsumer(duk_context *ctx) ;
	static duk_ret_t js_registerProcessor(duk_context *ctx) ;
	static duk_ret_t js_publish(duk_context *ctx) ;
	static duk_ret_t js_ack(duk_context *ctx) ;
	static duk_ret_t js_log(duk_context *ctx) ;



	void registerProcessor(std::string topic);
	void registerConsumer(std::string topic);
	void publish(std::string eventData);
	void ack(std::string eventData);
};


}

#endif // __DUKTAPEENGINE__
