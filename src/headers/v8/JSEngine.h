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

#ifndef __JSENGINE__
#define __JSENGINE__

#include "v8.h"
#include <iostream>
#include <functional>
#include <memory>
#include <fstream>
#include <streambuf>

#include "events/EventSystem.h"
#include "logger/Logger.h"
#include "apiserver/ApiClient.h"

namespace Susi {

using namespace v8;

class JSEngine{
private:

	static void Log(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void Publish(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void RegisterConsumer(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void RegisterProcessor(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void Acknowledge(const v8::FunctionCallbackInfo<v8::Value>& args);
protected:
	Isolate* isolate;
	Isolate::Scope isolate_scope;
    HandleScope handle_scope;
    Handle<Context> context;
    Context::Scope context_scope;
    Susi::Api::ApiClient api_client;
public:
	JSEngine(std::string file="")
	: isolate{Isolate::New()},
	  isolate_scope{isolate},
	  handle_scope{isolate},
	  context{Context::New(isolate)},
	  context_scope{context}{
	  	context->Global()->Set(v8::String::NewFromUtf8(isolate, "log"),
              v8::FunctionTemplate::New(isolate, Susi::JSEngine::Log)->GetFunction());
	  	context->Global()->Set(v8::String::NewFromUtf8(isolate, "publish"),
              v8::FunctionTemplate::New(isolate, Susi::JSEngine::Publish)->GetFunction());
	  	context->Global()->Set(v8::String::NewFromUtf8(isolate, "registerConsumer"),
              v8::FunctionTemplate::New(isolate, Susi::JSEngine::RegisterConsumer)->GetFunction());
	  	context->Global()->Set(v8::String::NewFromUtf8(isolate, "registerProcessor"),
              v8::FunctionTemplate::New(isolate, Susi::JSEngine::RegisterProcessor)->GetFunction());
	  	context->Global()->Set(v8::String::NewFromUtf8(isolate, "acknowledge"),
              v8::FunctionTemplate::New(isolate, Susi::JSEngine::Acknowledge)->GetFunction());
	  	runFile(file);
	}

	Handle<Value> runFile(std::string filename);

	Handle<Value> run(std::string code);

	Poco::Dynamic::Var convertFromJS(Handle<Value> jsVal);
	Handle<Value> convertFromCPP(Poco::Dynamic::Var cppVal);
	Handle<Value> convertFromCPP(std::string cppVal);
};

}

#endif // __JSENGINE__
