#include "apiserver/TCPServer.h"

Susi::Api::TCPServer::TCPServer(unsigned short port){
	base = event_base_new();
    if (!base) {
        throw std::runtime_error{"cant create libevent base"};
    }

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(port);

    auto accept_conn_cb = [](struct evconnlistener *listener,
							 evutil_socket_t fd, 
							 struct sockaddr *address, 
							 int socklen,
							 void *ctx){

    	TCPServer *self = (TCPServer*)ctx;
    	struct Context *context = new struct Context;
    	context->sessionID = std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
		context->server = self;

		struct event_base *base = evconnlistener_get_base(listener);
		struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
        self->outputs[context->sessionID] = bufferevent_get_output(bev);
		self->sockets[context->sessionID] = fd;
		
		self->onConnect(context->sessionID);

		auto read_cb = [](struct bufferevent *bev, void *ctx){
			struct Context * context = (struct Context *)ctx;
			std::string & sessionID = context->sessionID;
			auto server = context->server; 
			std::string data = "";
	        while(true){
	        	char buff[1024];
	        	size_t bs = bufferevent_read(bev, buff, sizeof(buff));
	        	if(bs<=0)break;
	        	data += std::string(buff,bs);
	        }
	        server->onData(sessionID,data);
		};
		auto event_cb = [](struct bufferevent *bev, short events, void *ctx){
			struct Context * context = (struct Context *)ctx;
			std::string & sessionID = context->sessionID;
			auto server = context->server; 
			if (events & BEV_EVENT_ERROR) std::cerr<<"Error from bufferevent"<<std::endl;
			if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
				bufferevent_free(bev);
				server->outputs.erase(sessionID);
				server->onClose(sessionID);
				delete context;
			}
		};
		bufferevent_setcb(bev, read_cb, NULL, event_cb, context);
		bufferevent_enable(bev, EV_READ|EV_WRITE);
    };

    struct evconnlistener *listener = evconnlistener_new_bind(base, accept_conn_cb, this,
        LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE, -1,
        (struct sockaddr*)&sin, sizeof(sin));
    
    if (!listener) {
        throw std::runtime_error{"cant create listener"};
    }
    evconnlistener_set_error_cb(listener, [](struct evconnlistener *listener, void *ctx){
    	struct event_base *base = evconnlistener_get_base(listener);
        int err = EVUTIL_SOCKET_ERROR();
        std::cerr<<"Got an error "<<err<<"\n shut down..."<<std::endl;
        event_base_loopexit(base, NULL);
    });
    runloop = std::move(std::thread{[this](){
		event_base_dispatch(base);
	}});
}


void Susi::Api::TCPServer::send(std::string & sessionID, std::string & data){
	auto output = outputs[sessionID];
	evbuffer_add(output,data.c_str(),data.size());
}

void Susi::Api::TCPServer::close(std::string & sessionID){
	auto fd = sockets[sessionID];
	evutil_closesocket(fd);
	onClose(sessionID);
}

void Susi::Api::TCPServer::join(){
	runloop.join();
}

void Susi::Api::TCPServer::stop(){
	event_base_loopexit(base, NULL);
}

Susi::Api::TCPServer::~TCPServer(){
	stop();
	join();
}