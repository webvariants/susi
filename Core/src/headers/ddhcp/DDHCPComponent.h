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
		Poco::Net::SocketAddress _localAddr;
		Poco::Net::DatagramSocket _serverSocket;
		Poco::Net::SocketAddress _lastPacketAddr;

		std::shared_ptr<std::thread> _runLoop;
		std::atomic<bool> _stop{false};
		char _buff[1<<16];

		std::string recv(){
			size_t bs = _serverSocket.receiveFrom(_buff,sizeof(_buff),_lastPacketAddr);
			return std::string{_buff,bs};	
		}

		void awnser(const std::string & msg){
			_serverSocket.sendTo(msg.c_str(),msg.size(),_lastPacketAddr);
		}

	public:
		DDHCPComponent(Susi::System::ComponentManager * mgr, unsigned short port) : 
		  Susi::System::BaseComponent{mgr}, 
		  _localAddr{Poco::Net::IPAddress(),port}, 
		  _serverSocket{_localAddr} {
			_serverSocket.setReceiveTimeout(Poco::Timespan(0,100000));
		}
		virtual void start() override {
			_runLoop.reset(new std::thread{[this](){
				try{
					std::string message;
					while(!_stop.load()){
						try{
							message = recv();
							LOG(DEBUG) << "got ddhcp message";
							awnser(message);
						}catch(const Poco::TimeoutException & e){}
					}
				}catch(const std::exception & e){
					LOG(ERROR) << std::string{"DDHCP: "}+e.what();
				}
			}});
			LOG(INFO) << "started DDHCP Component on addr "+_serverSocket.address().toString();
		}
		virtual void stop() override {
			_serverSocket.close();
			_stop.store(true);
			_runLoop->join();
			LOG(INFO) << "stopped DDHCP Component";
		}
	};

}
}

#endif // __DDHCPCOMPONENT__
