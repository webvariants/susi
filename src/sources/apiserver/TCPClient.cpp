#include "apiserver/TCPClient.h"

Susi::Api::TCPClient::TCPClient(std::string host, unsigned short port) {
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
	runloop = std::move(std::thread{[this](){
		event_base_dispatch(base);
	}});
}

void Susi::Api::TCPClient::send(std::string & data){
	bufferevent_write(bev,data.c_str(),data.size());
}

void Susi::Api::TCPClient::close(){
	evutil_socket_t fd = bufferevent_getfd(bev);
	evutil_closesocket(fd);
	event_base_loopexit(base, NULL);
	onClose();
}

void Susi::Api::TCPClient::join(){
	runloop.join();
}

void Susi::Api::TCPClient::stop(){
	event_base_loopexit(base, NULL);
}

Susi::Api::TCPClient::~TCPClient(){
	stop();
	join();
}
