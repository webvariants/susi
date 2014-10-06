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

#ifndef __DDHCPCOMPONENT__
#define __DDHCPCOMPONENT__

#include "world/BaseComponent.h"

#include <Poco/Net/DatagramSocket.h>

namespace Susi {
namespace Ddhcp {

	class DDHCPComponent : public Susi::System::BaseComponent {
	protected:
		Poco::Net::DatagramSocket _socket;
	public:
		DDHCPComponent(Susi::System::ComponentManager * mgr, unsigned short port) : Susi::System::BaseComponent{mgr} {
			_socket.bind(Poco::Net::SocketAddress{"localhost",port})
		}
		virtual void start() override {

		}
		virtual void stop() override {
			
		}
	}

}
}

#endif // __DDHCPCOMPONENT__
