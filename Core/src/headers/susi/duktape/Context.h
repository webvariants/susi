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
#include "bson/Value.h"

namespace Duktape {

	class Context {
	public:
		Context();
		Context(std::string filename);
		virtual ~Context();
	protected:
		duk_context *ctx = nullptr;

		void registerFunction(std::string name, NativeFunction function);
		void unregisterFunction(std::string name);
		
		void evalFile(std::string file);
		void evalSource(std::string source);

		BSON::Value getArgument(duk_context *ctx, int idx);
		int getArgumentCount(duk_context *ctx);
	};
}

#endif // __DUKTAPECONTEXT__
