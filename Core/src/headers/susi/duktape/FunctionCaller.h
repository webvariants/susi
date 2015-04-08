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

#ifndef __DUKTAPEFUNCTIONCALLER__
#define __DUKTAPEFUNCTIONCALLER__

#include "susi/duktape/duktape.h"
#include <map>
#include <functional>
#include <string>

namespace Duktape {

	typedef std::function<duk_ret_t(duk_context*)> NativeFunction;

	class FunctionCaller {
	protected:
		std::map<std::string,NativeFunction> functions;
	public:
		void registerFunction(std::string name, NativeFunction function){
			functions[name] = function;
		}
		void unregisterFunction(std::string name){
			functions.erase(name);
		}
		duk_ret_t call(std::string name, duk_context * ctx){
			try{
				return (functions[name])(ctx);
			}catch(const std::exception & e){
				//LOG(ERROR) << "can't find function '"+name+"' in js engine";
			}
			return 1;
		}
	};

	extern FunctionCaller functionCaller;

	static duk_ret_t nativeCall(duk_context * ctx){
		int args = duk_get_top(ctx);
		std::string function = duk_require_string(ctx, -args);
		return functionCaller.call(function,ctx);
	}
}

#endif // __DUKTAPEFUNCTIONCALLER__
