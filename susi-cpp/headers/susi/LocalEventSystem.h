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

#ifndef __LOCALEVENTSYSTEM__
#define __LOCALEVENTSYSTEM__

namespace Susi {

	typedef std::unique_ptr<BSON::Value> ProcessorEvent;
	typedef std::shared_ptr<BSON::Value> ConsumerEvent;

	typedef std::function<void(ProcessorEvent &&)> Processor;
	typedef std::function<void(ConsumerEvent const &)> Consumer;

	class LocalEventSystem {
	public:
		uint64_t registerProcessor(const std::string & topic, Processor);
		uint64_t registerConsumer(const std::string & topic, Consumer);
		void publish(ProcessorEvent && event);
		void ack(ProcessorEvent && event);

	protected:
		// id -> pair<topic,processor>
		std::map<std::uint64_t,std::pair<std::string,Processor>> processors
		// id -> pair<topic,consumer>
		std::map<std::uint64_t,std::pair<std::string,Consumer>> consumers;
		
		uint64_t generateId(){
			return std::chrono::system_clock::now().time_since_epoch().count();
		}

		struct PublishProcess {
			std::vector<Processor&> processors;
			std::vector<Consumer&> consumers;
			size_t nextProcessor = 0;
		};

		// event[id] -> PublishProcess
		std::map<std::string,PublishProcess> publishProcesses;

	};

}

#endif // __LOCALEVENTSYSTEM__
