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

	static Handle<Value> Log(const Arguments& args);
	static Handle<Value> Publish(const Arguments& args);
	static Handle<Value> RegisterConsumer(const Arguments& args);
	static Handle<Value> RegisterProcessor(const Arguments& args);

	static Susi::Util::Any convertFromJS(Handle<Value> jsVal);
	static Handle<Value> convertFromCPP(Susi::Util::Any cppVal);

protected:
	Isolate* isolate;
	Isolate::Scope isolate_scope;
    HandleScope handle_scope;
    Persistent<Context> context;
 	v8::Context::Scope context_scope;
    Susi::Api::ApiClient susi_client;

    Persistent<Context> createContext(){
		Local<ObjectTemplate> global_templ = ObjectTemplate::New();
		global_templ->SetAccessor(String::New("x"), XGetter, XSetter);
		global_templ->SetAccessor(String::New("y"), YGetter, YSetter);
		Persistent<Context> context = Context::New(NULL, global_templ);
	}	

public:
	Engine(std::string addr, std::string file="")
	: isolate{Isolate::New()},
	  isolate_scope{isolate},
	  context{},
	  context_scope{context},
	  susi_client{addr}
	{
		Susi::JS::engine = this;
		Persistent<Object> global = context->Global();
		//v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();
	  	global->Set(v8::String::New("log"),
              v8::FunctionTemplate::New(Susi::JS::Engine::Log));
	  	global->Set(v8::String::New("publish"),
              v8::FunctionTemplate::New(Susi::JS::Engine::Publish));
	  	global->Set(v8::String::New("registerConsumer"),
              v8::FunctionTemplate::New(Susi::JS::Engine::RegisterConsumer));
	  	global->Set(v8::String::New("registerProcessor"),
              v8::FunctionTemplate::New(Susi::JS::Engine::RegisterProcessor));
	  	
	  	runFile(file);
	}

	Handle<Value> runFile(std::string filename);
	Handle<Value> run(std::string code);

};


}
}

#endif // __V8ENGINE__
