/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 * 
 * @author: Thomas Krause (thomas.krause@webvariants.de)
 */

#ifndef __DEBUGGER_ENGINE__
#define __DEBUGGER_ENGINE__

#include "engine/Engine.h"

namespace Debugger {
	class Engine : public Susi::Cpp::Engine {

		protected:

		public:
			Engine(std::string address = "[::1]:4000") : Susi::Cpp::Engine{address} {};

			void addController(std::string name, Susi::Cpp::BaseController *c) {
				Susi::Cpp::Engine::addController(name, c);
			}

	};

}

#endif // __DEBUGGER_ENGINE__
