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

#ifndef __SUSICLIENTSTANDALONE__
#define __SUSICLIENTSTANDALONE__


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/times.h>
#include <resolv.h>
#include <arpa/nameser.h>
#include <string.h>
#include <unistd.h>

#include <string>
#include <iostream>
#include <deque>
#include <functional>
#include <map>
#include <vector>

class SusiClient {
protected:
	int 		sockfd;
	int			port;
	std::string	host;

	std::string buffer = "";
	int opening = 0;
	int closing = 0;
	std::deque<std::string> objects;

	std::map<std::string,std::vector<std::function<void(std::string&)>>> callbacks;

	bool connect(){
		int n;
		struct addrinfo hints, *res, *ressave;
		bzero(&hints, sizeof(struct addrinfo));
		hints.ai_family=AF_UNSPEC;
		hints.ai_socktype=SOCK_STREAM;
		hints.ai_protocol=IPPROTO_TCP;
		char * remote = (char*) host.c_str();
		char * port = (char*) std::to_string(this->port).c_str();
		if((n=getaddrinfo(remote, port, &hints, &res))!=0)
			printf("getaddrinfo error for %s, %s; %s", remote, port, gai_strerror(n));
		ressave=res;
		do{
			sockfd=socket(res->ai_family, res->ai_socktype, res->ai_protocol);
			if(sockfd<0)
				continue;  /*ignore this returned Ip addr*/
			if(::connect(sockfd, res->ai_addr, res->ai_addrlen)==0){ 
				freeaddrinfo(ressave);
				printf("connection ok!\n"); /* success*/
				return true;
			}
			else  {
				perror("connecting stream socket");
			}
			close(sockfd);/*ignore this one*/
		} while ((res=res->ai_next)!= NULL);
		freeaddrinfo(ressave);
		return false;
	}
	bool recv(){
		int bs;
		char buff[1024];
		if( (bs = read(sockfd,buff,1024)) > 0 ){
			std::string part = std::string(buff,bs);
			for(int i=0;i<part.length();i++){
				char c = part[i];
				buffer += c;
				if(c == '{')opening++;
				if(c == '}'){
					closing++;
					if(opening == closing){
						objects.push_back(buffer);
						buffer = "";
						opening = 0;
						closing = 0;
					}
				}
			}
			return true;
		}
		return false;
	}
	bool send(std::string & msg){
		int bs = write(sockfd,msg.c_str(),msg.length());
		return bs==msg.length();
	}
	bool recvJson(std::string & msg){
		while(objects.empty()){
			if(!recv())return false;
		}
		msg = objects[0];
		objects.pop_front();
		return true;
	}
public:
	SusiClient(std::string host, int port) : host(host), port(port) {
		connect();
	}
	bool publish(std::string topic,std::string payload,std::string returnAddr){
		std::string msg = "{\"type\":\"publish\",\"key\":\""+topic+"\",\"payload\":\""+payload+"\",\"returnaddr\":\""+returnAddr+"\"}";
		return send(msg);
	}
	bool subscribe(std::string topic,std::function<void(std::string&)> callback){
		callbacks[topic].push_back(callback);
		std::string msg = "{\"type\":\"subscribe\",\"key\":\""+topic+"\"}";
		return send(msg);
	}
	void getEvent(){
		std::string msg;
		while(true){
			if(!recvJson(msg)){
				return;
			}
			if(msg.find("\"type\" : \"event\"") != std::string::npos){
				for(auto & kv : callbacks){
					if(msg.find("\"key\" : \""+kv.first+"\"")){
						for(auto & callback : kv.second){
							callback(msg);
						}
						return;
					}
				}
			}
		}
	}
};

/*
int main(int argc, char** argv){
	SusiClient client("::1",4000);
	client.subscribe("foo",[](std::string & msg){
		std::cout<<msg;
	});
	client.publish("foo","bar1","");
	client.publish("foo","bar2","");
	client.publish("foo","bar3","");
	client.getEvent();
	client.getEvent();
	client.getEvent();
}
*/

#endif // __SUSICLIENTSTANDALONE__