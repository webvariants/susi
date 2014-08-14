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

#ifndef __POCOTCPCLIENT__
#define __POCOTCPCLIENT__

#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/SocketAddress.h>
#include <thread>

namespace Susi {
namespace Api {

	class TCPClient {
	public:
		
		virtual void onConnect() = 0;
		virtual void onData(std::string & data) = 0;
		virtual void onClose() = 0;
		
		TCPClient(std::string address) : sa{address}, sock{sa} {
			runloop = std::move(std::thread{[this](){
				onConnect();
				char buff[1024];
				size_t bs;
				while(true){
					bs = sock.receiveBytes(buff,bs);
					if(bs<=0){
						onClose();
						break;
					}
					std::string data{buff,bs};
					onData(data);
				}
			}});
		}
		
		void close(){
			try{
				sock.shutdown();
			}catch(...){}
		}
		void send(std::string msg){
			sock.sendBytes(msg.c_str(),msg.size());
		}
		virtual ~TCPClient(){
			close();
			runloop.join();
		}

	private:
		Poco::Net::SocketAddress sa;
    	Poco::Net::StreamSocket sock;
    	std::thread runloop;
	};

}
}

#endif // __POCOTCPCLIENT__
