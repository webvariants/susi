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

#ifndef __HEARTBEAT__
#define __HEARTBEAT__

#include "events/global.h"
#include <thread>

namespace Susi {
	class HeartBeat {
	protected:
		std::thread t;
		bool _stop = false;
	public:
		HeartBeat() : t{[this](){
			int count = 0;
			std::chrono::seconds interval(1);
			auto event_one = Susi::Events::createEvent("heartbeat::one");
			auto event_five = Susi::Events::createEvent("heartbeat::five");
			auto event_ten = Susi::Events::createEvent("heartbeat::ten");
			auto event_minute = Susi::Events::createEvent("heartbeat::minute");
			auto event_five_minute = Susi::Events::createEvent("heartbeat::fiveMinute");
			while(!this->_stop){
				++count %= 300;
				Susi::Events::publish(std::move(event_one));
				if(count % 5 == 0){
					Susi::Events::publish(std::move(event_five));
				}
				if(count % 10 == 0){
					Susi::Events::publish(std::move(event_ten));
				}
				if(count % 60 == 0){
					Susi::Events::publish(std::move(event_minute));
				}
				if(count % 300 == 0){
					Susi::Events::publish(std::move(event_five_minute));
				}
				std::this_thread::sleep_for(interval);
			}
		}} {}
		~HeartBeat(){
			_stop = true;
			t.join();
		}
	};
}

#endif // __HEARTBEAT__
