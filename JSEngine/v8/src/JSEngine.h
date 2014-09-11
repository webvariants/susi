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

namespace Susi {
namespace JS {
namespace V8 {

using namespace v8;

class Engine{
static Susi::Util::Any fromJS(Handle<Value> value) {
	HandleScope scope;

	Handle<Context> context = Context::GetCurrent();
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

static Handle<Value> Log(const Arguments& args) {
	Susi::Util::Any cppvalue = fromJS(args[0]);
	Handle<Value> jsvalue = fromCPP(cppvalue);
	String::Utf8Value str(jsvalue);
	Susi::Util::Any cppvalue_2 = fromJS(jsvalue);
	Susi::Logger::log("From JS to CPP: "+cppvalue.toString());
	Susi::Logger::log("Back to JS: "+ std::string(*str));
	Susi::Logger::log("From JS to CPP again: "+cppvalue_2.toString());
	Susi::Logger::log("ALL IN ONE: "+fromJS(fromCPP(fromJS(args[0]))).toString());

	return Undefined();
}

protected:
	 // Executes a string within the current v8 context.
	bool run(Handle<String> source) {
		TryCatch try_catch;
		bool return_val = false;
		Persistent<Context> context{this->setupContext()};
		if (context.IsEmpty()) {
			Susi::Logger::log("JS::Engine.run: Error creating context");
			return_val = false;
	    }
	    else {
			context->Enter();
			Context::Scope context_scope{context};
			Handle<Script> script = Script::New(source, String::New("script"));
			if (script.IsEmpty()) {
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
    		context.Dispose();
	    }

		return return_val;
	}

	Persistent<Context> setupContext() {
		// Create a template for the global object.
		Handle<ObjectTemplate> global{ObjectTemplate::New()};
		// Bind the global 'print' function to the C++ Print callback.
		global->Set(String::New("log"), FunctionTemplate::New(Susi::JS::V8::Engine::Log));

		return Context::New(NULL, global);
	}

public:
	Engine(){}

	bool run(std::string source) {
		HandleScope handle_scope;
		return this->run(String::New(source.c_str()));
	}

	~Engine() {}
};


}
}
}

#endif // __V8ENGINE__
