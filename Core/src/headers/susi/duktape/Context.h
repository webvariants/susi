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

#ifndef __DUKTAPECONTEXT__
#define __DUKTAPECONTEXT__

#include "susi/duktape/FunctionCaller.h"
#include <stdexcept>

namespace Duktape {

	class Context {
	protected:
		duk_context *ctx = nullptr;
		void registerFunction(std::string name, NativeFunction function){
			functionCaller.registerFunction(name,function);
		}
		void unregisterFunction(std::string name){
			functionCaller.unregisterFunction(name);
		}
		void evalFile(std::string file){
			if(file != ""){
				if (duk_peval_file(ctx, file.c_str()) != 0) {
					throw std::runtime_error{std::string{"failed executing js sourcefile: "}+duk_safe_to_string(ctx, -1)};
				}
			}
		}
		void evalSource(std::string source){
			if(duk_peval_lstring(ctx, source.c_str(), source.size()) != 0){
				throw std::runtime_error{std::string{"failed executing js source: "}+duk_safe_to_string(ctx, -1)};
			}
		}	
	public:
		Context(){
			ctx = duk_create_heap_default();
			if (!ctx) {
				throw std::runtime_error{"failed creating duktape context"};
			}
			duk_push_global_object(ctx);
			duk_push_c_function(ctx, nativeCall, DUK_VARARGS);
			duk_put_prop_string(ctx, -2, "__nativeCall");
		}
		Context(std::string filename) : Context() {
			evalFile(filename);
		}
	};
}

#endif // __DUKTAPECONTEXT__
