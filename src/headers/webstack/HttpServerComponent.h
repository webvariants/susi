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

#ifndef __HTTP_SERVER_COMPONENT__
#define __HTTP_SERVER_COMPONENT__

#include "world/BaseComponent.h"
#include "webstack/HttpServer.h"

namespace Susi {
	class HttpServerComponent : Susi::System::BaseComponent ,  HttpServer {
		public:
			HttpServerComponent (Susi::System::ComponentManager * mgr, std::string addr,std::string assetRoot) : 
				Susi::System::BaseComponent{mgr}, HttpServer{addr, assetRoot} {}

			virtual void start() override {
				httpServerStart();
			}

			virtual void stop() override {
				httpServerStop();
			}
			~HttpServerComponent() {
				stop();
			}
	};
}

#endif // __HTTP_SERVER_COMPONENT__
