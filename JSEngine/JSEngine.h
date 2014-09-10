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

#include "logger/Logger.h"
#include "util/Any.h"
#include "apiserver/ApiClient.h"

namespace Susi {
namespace JS {

using namespace v8;

class Engine;
extern Engine *engine;

class Engine{
private:

	static void Log(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void Publish(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void RegisterConsumer(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void RegisterProcessor(const v8::FunctionCallbackInfo<v8::Value>& args);

	static Susi::Util::Any convertFromJS(Handle<Value> jsVal);
	static Handle<Value> convertFromCPP(Susi::Util::Any cppVal);
	static Handle<Value> convertFromCPP(std::string cppVal);

protected:
	Isolate* isolate;
	Isolate::Scope isolate_scope;
    HandleScope handle_scope;
    Handle<Context> context;
    Context::Scope context_scope;
    Susi::Api::ApiClient susi_client;

public:
	Engine(std::string addr, std::string file="")
	: isolate{Isolate::New()},
	  isolate_scope{isolate},
	  handle_scope{isolate},
	  context{Context::New(isolate)},
	  context_scope{context},
	  susi_client{addr}
	{
		Susi::JS::engine = this;
	  	context->Global()->Set(v8::String::NewFromUtf8(isolate, "log"),
              v8::FunctionTemplate::New(isolate, Susi::JS::Engine::Log)->GetFunction());
	  	context->Global()->Set(v8::String::NewFromUtf8(isolate, "publish"),
              v8::FunctionTemplate::New(isolate, Susi::JS::Engine::Publish)->GetFunction());
	  	context->Global()->Set(v8::String::NewFromUtf8(isolate, "registerConsumer"),
              v8::FunctionTemplate::New(isolate, Susi::JS::Engine::RegisterConsumer)->GetFunction());
	  	context->Global()->Set(v8::String::NewFromUtf8(isolate, "registerProcessor"),
              v8::FunctionTemplate::New(isolate, Susi::JS::Engine::RegisterProcessor)->GetFunction());
	  	runFile(file);
	}

	Local<Value> runFile(std::string filename);
	Local<Value> run(std::string code);

};


}
}

#endif // __V8ENGINE__
