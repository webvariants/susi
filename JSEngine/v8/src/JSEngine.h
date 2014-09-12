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

#include "apiserver/ApiClient.h"
#include "logger/Logger.h"
#include "util/Any.h"

namespace Susi {
namespace JS {
namespace V8 {

using namespace v8;

class Engine{
protected:
	static Persistent<Context> setupContext() {
		// Create a template for the global object.
		Handle<ObjectTemplate> global{ObjectTemplate::New()};
		// Bind the global 'print' function to the C++ Print callback.
		global->Set(String::New("log"), FunctionTemplate::New(Susi::JS::V8::Engine::Log));
		global->Set(String::New("publish"), FunctionTemplate::New(Susi::JS::V8::Engine::Publish));

		return Context::New(NULL, global);
	}

	static Susi::Util::Any fromJS(Handle<Value> value) {
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

	static Handle<Value> fromCPP(Susi::Util::Any value) {
		HandleScope scope;

		Handle<Context> context = Context::GetCurrent();
	    Handle<Object> global = context->Global();

		Handle<Object> JSON = global->Get(String::New("JSON"))->ToObject();
	    Handle<Function> JSON_parse = Handle<Function>::Cast(JSON->Get(String::New("parse")));

	    Handle<Value> valueHandle{String::New(value.toString().c_str())};

	    return scope.Close(JSON_parse->Call(JSON, 1, &valueHandle));
	}

	// Susi API Mapping

	static Handle<Value> Log(const Arguments& args) {
		Susi::Logger::log(fromJS(args[0]).toString());
		return Undefined();
	}

	static Handle<Value> Publish(const Arguments& args) {
		susi_api->publish(susi_api->createEvent(fromJS(args[0])));
		return Undefined();
	}

	// Executes a string within the current v8 context.
	bool run(Handle<String> source) {
		TryCatch try_catch;
		bool return_val = false;
		if (context.IsEmpty()) {
			Susi::Logger::log("JS::Engine.run: context is Empty");
			return_val = false;
	    }
	    else {
			context->Enter();
			Context::Scope context_scope{context};
			Handle<Script> script = Script::New(source, String::New("script"));
			if (script.IsEmpty()) {
				Susi::Logger::log("JS::Engine.run: Script is Empty");
				return_val = false;
			} else {
				Handle<Value> result = script->Run();
			    if (result.IsEmpty()) {
					return_val = false;
			    }
			    else {
			    	return_val = true;
			    }
			}
			context->Exit();
	    }

		return return_val;
	}

public:
	HandleScope scope;
	static Persistent<Context> context;
	static std::shared_ptr<Susi::Api::ApiClient> susi_api;

	Engine()
	{
		if(context.IsEmpty()) {
			Locker v8Locker;
			HandleScope scope;
			context = Persistent<Context>(setupContext());
			susi_api = std::shared_ptr<Susi::Api::ApiClient>{new Susi::Api::ApiClient("[::1]:4000")};
		}
	}

	bool run(std::string source) {
		Locker v8Locker;
		HandleScope scope;
		return this->run(String::New(source.c_str()));
	}

	~Engine() {
		context.Dispose();
	}
};

Persistent<Context> Engine::context = Persistent<Context>();
std::shared_ptr<Susi::Api::ApiClient> Engine::susi_api = nullptr;
}
}
}

#endif // __V8ENGINE__
