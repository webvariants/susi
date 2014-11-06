/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 *
 * @authors: Tino Rusch (tino.rusch@webvariants.de), Christian Sonderfeld (christian.sonderfeld@webvariants.de)
 */

#ifndef __V8ENGINE__
#define __V8ENGINE__

#include "v8.h"
#include <iostream>
#include <functional>
#include <memory>
#include <fstream>
#include <streambuf>

#include "util/Channel.h"
#include "logger/easylogging++.h"
#include "util/Any.h"
#include "events/EventManager.h"
#include "apiserver/BasicApiClient.h"

namespace Susi {
namespace JS {
namespace V8 {

using namespace v8;

class Engine : public Susi::Api::BasicApiClient {
protected:

	Susi::Util::Channel<std::function<void()>> callback_channel;

	Persistent<Context> setupContext() {
		// Create a template for the global object.
		Handle<ObjectTemplate> global{ObjectTemplate::New()};
		// Bind the global 'print' function to the C++ Print callback.
		global->Set(String::New("log"), FunctionTemplate::New([] (const Arguments& args) -> Handle<Value>  {
				Handle<External> data = Handle<External>::Cast(args.Data());
				auto self = static_cast<Susi::JS::V8::Engine*>(data->Value());

				LOG(INFO) << self->fromJS(args[0]).toJSONString();
				return Undefined();
			},
			External::New(this)
		));


		global->Set(String::New("publish"), FunctionTemplate::New([] (const Arguments& args) -> Handle<Value>  {
				Handle<External> data = Handle<External>::Cast(args.Data());
				auto self = static_cast<Susi::JS::V8::Engine*>(data->Value());

				Susi::Events::Event evt{};
				Susi::Util::Any obj = self->fromJS(args[0]);
				if(obj.isObject()) {
					evt.setTopic(static_cast<std::string>(obj["topic"]));
					evt.setPayload(obj["payload"]);
				}
				else if(obj.isString()) {
					evt.setTopic(static_cast<std::string>(obj));
				}
				else {
					return ThrowException(String::New("publish: input has to be either a string or an object containing at least topic"));
				}
				if(args.Length() > 1) {
					self->finishedCbs[evt.id] = Persistent<Function>::New(Handle<Function>::Cast(args[1]));
				}

				self->sendPublish(evt);

				return True();
			},
			External::New(this)
		));


		global->Set(String::New("subscribeConsumer"), FunctionTemplate::New([] (const Arguments& args) -> Handle<Value> {
				Handle<External> data = Handle<External>::Cast(args.Data());
				auto self = static_cast<Susi::JS::V8::Engine*>(data->Value());

				if(args.Length() < 2) {
					LOG(INFO) << "JSEngine - subscribeConsumer: wrong number of Arguments";
					return Undefined();
				}
				Locker v8Locker;
				HandleScope scope;

				Susi::Util::Any value = self->fromJS(args[0]);
				std::string topic = value;

				Persistent<Function> callback = Persistent<Function>::New(Handle<Function>::Cast(args[1]));

				if(callback.IsEmpty()) {
					LOG(INFO) << "JSEngine - subscribeConsumer: Topic: "+topic+" , failed - callback not found";
					return ThrowException(String::New("subscribeConsumer: you have to specify a callback function as second argument"));
				}

				LOG(INFO) << "JSEngine - subscribeConsumer: Topic: "+topic;

				if(self->consumers.count(topic) == 0) {
					self->sendRegisterConsumer(topic);
				}
				self->consumers[topic].push_back(callback);

				return True();
			},
			External::New(this)
		));
		global->Set(String::New("subscribeProcessor"), FunctionTemplate::New([] (const Arguments& args) -> Handle<Value> {
				Handle<External> data = Handle<External>::Cast(args.Data());
				auto self = static_cast<Susi::JS::V8::Engine*>(data->Value());

				if(args.Length() < 2) {
					LOG(INFO) << "JSEngine - subscribeProcessor: wrong number of Arguments";
					return Undefined();
				}
				Locker v8Locker;
				HandleScope scope;

				Susi::Util::Any value = self->fromJS(args[0]);
				std::string topic = value;

				Persistent<Function> callback = Persistent<Function>::New(Handle<Function>::Cast(args[1]));

				if(callback.IsEmpty()) {
					LOG(INFO) << "JSEngine - subscribeProcessor: Topic: "+topic+" , failed - callback not found";
					return ThrowException(String::New("subscribeProcessor: you have to specify a callback function as second argument"));
				}

				LOG(INFO) << "JSEngine - subscribeProcessor: Topic: "+topic;

				if(self->processors.count(topic) == 0) {
					self->sendRegisterProcessor(topic);
				}
				self->processors[topic].push_back(callback);

				return True();
			},
			External::New(this)
		));

		return Context::New(NULL, global);
	}

	Susi::Util::Any fromJS(Handle<Value> value) {
		Locker v8Locker;
		HandleScope scope;

	    Handle<Object> global = context->Global();

		Handle<Object> JSON = global->Get(String::New("JSON"))->ToObject();
	    Handle<Function> JSON_stringify = Handle<Function>::Cast(JSON->Get(String::New("stringify")));

	    Handle<Value> stringifiedValue = JSON_stringify->Call(JSON, 1, &value);
	    if(stringifiedValue->IsString()) {
	    	String::Utf8Value stringValue{stringifiedValue};
	    	return Susi::Util::Any::fromString(std::string(*stringValue));
	    }
	    return Susi::Util::Any{};
	}

	Handle<Value> fromCPP(Susi::Util::Any value) {
		Locker v8Locker;
		HandleScope scope;

	    Handle<Object> global = Susi::JS::V8::Engine::context->Global();

		Handle<Object> JSON = global->Get(String::New("JSON"))->ToObject();
	    Handle<Function> JSON_parse = Handle<Function>::Cast(JSON->Get(String::New("parse")));

	    Handle<Value> valueHandle{String::New(value.toJSONString().c_str())};

	    return scope.Close(JSON_parse->Call(JSON, 1, &valueHandle));
	}

	virtual void onConsumerEvent(Susi::Events::Event & event) override{
		auto evt = std::make_shared<Susi::Events::Event>(event);
		std::function<void()> f = [this, evt]() {
			Locker v8Locker;
			HandleScope scope;
			std::vector<Persistent<Function>> consumer = consumers[evt->topic];
			Handle<Object> global{context->Global()};
			Handle<Value> e = fromCPP(evt->toAny());
			for (auto & c : consumer)
			{
				c->Call(global, 1, &e);
			}
		};

		callback_channel.put(f);
	}

	virtual void onProcessorEvent(Susi::Events::Event & event) override {
		auto evt = std::make_shared<Susi::Events::Event>(event);
		std::function<void()> f = [this, evt](){
			Locker v8Locker;
			HandleScope scope;
			std::vector<Persistent<Function>> processor = processors[evt->topic];
			Handle<Object> global{context->Global()};
			Handle<Value> e = fromCPP(evt->toAny());
			for (auto & c : processor)
			{
				e = c->Call(global, 1, &e);
			}
			Susi::Util::Any ev = fromJS(e);
			evt->setPayload(ev["payload"]);
			sendAck(*evt);
		};

		callback_channel.put(f);
	}

	virtual void onAck(Susi::Events::Event & event) override{
		auto evt = std::make_shared<Susi::Events::Event>(event);
		std::function<void()> f = [this, evt](){
			try {
				Locker v8Locker;
				HandleScope scope;
				Handle<Object> global{context->Global()};
				Handle<Value> e = fromCPP(evt->toAny());
				finishedCbs.at(evt->id)->Call(global, 1, &e);
				finishedCbs.erase(evt->id);
			}
			catch(...) {
			}
		};

		callback_channel.put(f);
	}

	bool run(Handle<String> source) {
		TryCatch try_catch;
		bool return_val = false;
		if (context.IsEmpty()) {
			LOG(INFO) << "JS::Engine.run: context is Empty";
			return_val = false;
	    }
	    else {
			context->Enter();
			Context::Scope context_scope{context};
			Handle<Script> script = Script::New(source, String::New("script"));
			if (script.IsEmpty()) {
				LOG(INFO) << "JS::Engine.run: Script is Empty";
				return_val = false;
			} else {
				Handle<Value> result = script->Run();
				if (result.IsEmpty()) {
					return_val = false;
			    }
			    else {
			    	return_val = true;
			    	while(true) {
			    		try {
			    			auto f = callback_channel.get();
			    			f();
			    		}
			    		catch(Susi::Util::ChannelClosedException e) {
			    			return return_val;
			    		}
			    	}
			    }
			}
	    }

		return return_val;
	}

public:
	Persistent<Context> context;
	std::map<std::string, std::vector<Persistent<Function>>> consumers;
	std::map<std::string, std::vector<Persistent<Function>>> processors;
	std::map<long, Persistent<Function>> finishedCbs;

	HandleScope scope;


	Engine(std::string address = "[::1]:4000")
		: BasicApiClient{address}
	{
		if(context.IsEmpty()) {
			Locker v8Locker;
			HandleScope scope;
			TryCatch try_catch;
			context = Persistent<Context>(setupContext());
			if(try_catch.HasCaught()) {
				String::Utf8Value exception{try_catch.Exception()};
				LOG(ERROR) << std::string(*exception);
			}
		}
	}

	// This function will block the thread (because of run)
	bool run(std::string source) {
		Locker v8Locker;
		HandleScope scope;
		return this->run(String::New(source.c_str()));
	}

	// This function will block the thread (because of run)
	bool runFile(std::string source) {
		std::ifstream t(source);
		std::stringstream buffer;
		buffer << t.rdbuf();

		Locker v8Locker;
		HandleScope scope;
		return this->run(String::New(buffer.str().c_str()));
	}

	void stop() {
		context->Exit();
		context.Dispose();
	}

	~Engine() {
		close();
		stop();
	}
};


}
}
}
#endif // __V8ENGINE__
