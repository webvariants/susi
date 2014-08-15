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
#include <iostream>
namespace Susi {
namespace Api {

	class TCPClient {
	public:
		
		//These functions are not pure virtual, because of destruction problems.
		virtual void onConnect() {};
		virtual void onData(std::string & data) {};
		virtual void onClose() {};
		
		TCPClient(std::string address) : sa{address}, sock{sa} {
			sock.setReceiveTimeout(Poco::Timespan{0,1000000});
			runloop = std::move(std::thread{[this](){
				onConnect();
				char buff[1024];
				int bs;
				while(!isClosed){
					try{
						std::cout<<"wait for bytes"<<std::endl;
						bs = sock.receiveBytes(buff,1024);
						std::cout<<"got "<<bs<<std::endl;
						if(bs<=0){
							onClose();
							break;
						}
						std::string data{buff,static_cast<size_t>(bs)};
						onData(data);
					}catch(const Poco::TimeoutException & e){
						std::cout<<"timeout!"<<std::endl;
						if(isClosed)break;
					}catch(const std::exception & e){
						std::cout<<"Exception: "<<e.what()<<std::endl;
						break;
					}
				}
				std::cout<<"runloop end"<<std::endl;
			}});
		}
		
		void close(){
			if(!isClosed){
				try{
					sock.shutdown();
					sock.close();
					isClosed = true;
					std::cout<<"isClosed = true" <<std::endl;
					runloop.join();
					onClose();
				}catch(const std::exception & e){
					std::cout<<"error while closing/joining: "<<e.what()<<std::endl;
					onClose();
				}
			}
		}
		void send(std::string msg){
			auto bs = sock.sendBytes(msg.c_str(),msg.size());
			std::cout<<"sended "<<bs<<"/"<<msg.size()<<" bytes"<<std::endl;
		}
		virtual ~TCPClient(){
			close();
		}

	private:
		bool isClosed = false;
		Poco::Net::SocketAddress sa;
    	Poco::Net::StreamSocket sock;
    	std::thread runloop;
	};

}
}

#endif // __POCOTCPCLIENT__
