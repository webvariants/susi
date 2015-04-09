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
#include "susi/world/BaseComponent.h"
#include "susi/duktape/Context.h"

namespace Susi {
namespace Duktape {

class JSEngine : public Susi::System::BaseComponent {
public:
	JSEngine(Susi::System::ComponentManager *mgr, std::string src = "", int _numEngines = 1) : 
		Susi::System::BaseComponent{mgr},
		sourceFile{src},
		numEngines{_numEngines} {}

	// The start function subscribes to events and attaches its handlers
	virtual void start() override;

	// When stopping, all subscriptions are deleted
	virtual void stop() override;

protected:
	int numEngines = 1;
	std::string sourceFile = "";

	class SusiAwareContext : public ::Duktape::Context {
	protected:
		std::shared_ptr<Susi::Events::IEventSystem> eventManager;

		duk_ret_t publish(duk_context *ctx);
		duk_ret_t registerConsumer(duk_context *ctx);
		duk_ret_t registerProcessor(duk_context *ctx);
		duk_ret_t ack(duk_context *ctx);

		void forwardAckToJS(Susi::Events::SharedEventPtr event);
		void forwardConsumerEventToJS(Susi::Events::SharedEventPtr event);
		void forwardProcessorEventToJS(Susi::Events::EventPtr event);

	public:
		SusiAwareContext(std::string sourceFile);
	};

	std::vector<std::shared_ptr<SusiAwareContext>> contexts;

};


}}

#endif // __DUKTAPEENGINE__
