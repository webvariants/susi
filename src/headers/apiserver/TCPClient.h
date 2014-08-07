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

#ifndef __TCPCLIENT__
#define __TCPCLIENT__

#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/dns.h>

#include <arpa/inet.h>
#include <thread>

namespace Susi{
namespace Api{

class TCPClient {
protected:
	struct event_base *base;
    struct bufferevent *bev;
    std::thread runloop;
    virtual void onConnect(){};
    virtual void onData(std::string & data) = 0;
    virtual void onClose(){};
public:
	TCPClient(std::string host, unsigned short port){
		base = event_base_new();
		bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);

		auto eventcb = [](struct bufferevent *bev, short events, void *ctx){
			TCPClient * client = (TCPClient*)ctx;
			if (events & BEV_EVENT_CONNECTED) {
				client->onConnect();
			} else if (events & (BEV_EVENT_ERROR|BEV_EVENT_EOF)) {
				struct event_base *base = client->base;
				if (events & BEV_EVENT_ERROR) {
					int err = bufferevent_socket_get_dns_error(bev);
					if (err)
						printf("DNS error: %s\n", evutil_gai_strerror(err));
				}
				client->onClose();
				printf("Closing\n");
				bufferevent_free(bev);
				event_base_loopexit(base, NULL);
			}else{
				std::cout<<"unknown event"<<std::endl;
			}
		};

		auto readcb = [](struct bufferevent *bev, void *ctx){
			TCPClient * client = (TCPClient*)ctx;
			std::string data = "";
	        while(true){
	        	char buff[1024];
	        	size_t bs = bufferevent_read(bev, buff, sizeof(buff));
	        	if(bs<=0)break;
	        	data += std::string(buff,bs);
	        }
	        client->onData(data);
		};

		bufferevent_setcb(bev, readcb, NULL, eventcb, this);
    	bufferevent_enable(bev, EV_READ|EV_WRITE);
		
		struct evdns_base *dns_base = evdns_base_new(base, 1);
    	bufferevent_socket_connect_hostname(bev, dns_base, AF_UNSPEC, host.c_str(), port);
	}

	void send(std::string data){
		bufferevent_write(bev,data.c_str(),data.size());
	}

	void close(){
		evutil_socket_t fd = bufferevent_getfd(bev);
		evutil_closesocket(fd);
		event_base_loopexit(base, NULL);
		onClose();
	}

	void run(){
		runloop = std::move(std::thread{[this](){
			event_base_dispatch(base);
		}});
	}

	void join(){
		runloop.join();
	}

	void stop(){
		event_base_loopexit(base, NULL);
	}

	virtual ~TCPClient(){
		stop();
		join();
	}

};

}
}

#endif // __TCPCLIENT__