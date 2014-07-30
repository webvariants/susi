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

#ifndef __EVENTMANAGER__
#define __EVENTMANAGER__

#include <chrono>
#include <utility>
#include <Poco/Dynamic/Var.h>
#include <functional>
#include <memory>
#include <map>
#include <vector>
#include <mutex>
#include <thread>

#include <iostream>

#include "util/Channel.h"

namespace Susi{
namespace Events{

	typedef std::pair<std::string,std::string> Header;

	struct Event {
		long id;
		std::string topic;
		std::vector<Header> headers;
		Poco::Dynamic::Var payload;
	};

	typedef std::unique_ptr<Event,std::function<void(Event*)>> EventPtr; 
	typedef std::function<void( EventPtr )> Handler;
	typedef std::function<bool( Event & )> Predicate;


	class Manager {
	friend class AcknowledgeWorker;
	protected:
		struct PublishProcess {
			std::vector<Handler> handlers;
			std::size_t current;
			std::vector<std::string> errors;
			Handler finishHandler;
		};

		// map from EventID to PublishProcess
		std::map<long,std::shared_ptr<PublishProcess>> publishProcesses;
		// map from topic to list of pairs of subscription-id and handler;
		std::map<std::string,
				 std::vector<
				 	std::pair<
				 		long,
				 		Handler>>> handlersByTopic;
		// list of tuple of subscription-id, predicate and handler;
		std::vector<std::tuple<
			long,
			Predicate,
			Handler>> handlersByPred;

		std::mutex publishProcessesMutex;
		std::mutex handlersByTopicMutex;
		std::mutex handlersByPredMutex;

		std::shared_ptr<Susi::Util::Channel<EventPtr>> workChannel;

		class AcknowledgeWorker {
			protected:
				Manager * _manager;
				std::shared_ptr<Susi::Util::Channel<EventPtr>> _workChannel;
			public:
				AcknowledgeWorker(Manager * manager, std::shared_ptr<Susi::Util::Channel<EventPtr>> workChannel) : 
					_manager{manager}, _workChannel{workChannel} {}
				void run();
		};

		std::vector<std::thread> workers;

	public:
		Manager();
		//register handler by topic. returns subscription-id
		long subscribe(std::string topic, Handler handler);
		//register handler by predicate. returns subscription-id
		long subscribe(Predicate pred, Handler handler);
		// unsubscribe by id
		bool unsubscribe(long id);

		//publish an event. collects all handlers and starts a publish process
		void publish(EventPtr event, Handler finishCallback);
		// acknowledges an event. tells the eventmanager to use next handler
		void ack(EventPtr event);

		~Manager(){
			workChannel->close();
			//std::cout<<"destructing eventManager"<<std::endl;
			for(auto & t : workers){
				t.join();
			}
		}
	};

}
}

#endif // __EVENTMANAGER__
