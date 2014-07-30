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

#ifndef __CONNECTION__
#define __CONNECTION__

#include <Poco/Dynamic/Var.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/SocketStream.h>
#include <Poco/Net/WebSocket.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Net/TCPServerConnection.h>

#include <iostream>
#include <mutex>
#include <locale>
#include <algorithm>

#include "logger/Logger.h"
#include "events/EventSystem.h"

namespace Susi {

	/**
	 * The Base class for API server connections.
	 */
	class Connection {
	public:
		/**
		 * This has to be implemented by subclasses
		 */
		virtual bool send(Poco::Dynamic::Var & data) = 0;
		virtual bool recv(Poco::Dynamic::Var & data) = 0;
		virtual void close() = 0;
		virtual ~Connection();

		bool send(Poco::Dynamic::Var && data){
			return send(data);
		};
		/**
		 * Setter and getter for sessionID
		 */
		std::string getSessionID(){return sessionID;}
		void setSessionID(std::string sessionID){this->sessionID = sessionID;}

		/**
		 * Subscribe this connection to topic.
		 * Check for subscriptions, subscribe at system, memorize it, report to client side.
		 * @param topic
		 */
		void subscribe(std::string topic,int requestID);
		void unsubscribe(std::string topic,int requestID);

	protected:
		std::string sessionID = "";
		std::vector<std::pair<long,std::string>> subscriptions;
	};


	class TCPConnection : public Connection {
	public:
		TCPConnection(const Poco::Net::StreamSocket & sock) 
			: socket(sock), 
			  ss(sock) {};
		virtual bool send(Poco::Dynamic::Var & data) override;
		virtual bool recv(Poco::Dynamic::Var & data) override;
		virtual void close(){
			try{
				socket.shutdown();
			}catch(const std::exception & e){}
		}


		bool send(Poco::Dynamic::Var && data){
			return send(data);
		};

		~TCPConnection() {
			close();
		}
	private:
		Poco::Net::StreamSocket socket;
		Poco::Net::SocketStream ss;
		std::mutex mutex;
		int countInString(std::string s, char c);
	};

	class WebSocketConnection : public Connection {
	public:
		WebSocketConnection(Poco::Net::WebSocket socket) : socket(socket) {};
		virtual bool send(Poco::Dynamic::Var & data) override;
		virtual bool recv(Poco::Dynamic::Var & data) override;
		virtual void close() override{
			try{
				socket.shutdown();
			}catch(const std::exception & e){}
		}
		bool send(Poco::Dynamic::Var && data){
			return send(data);
		};
		~WebSocketConnection(){
			close();
		}
	private:
		Poco::Net::WebSocket socket;
		std::mutex mutex;
	};

}

#endif // __CONNECTION__