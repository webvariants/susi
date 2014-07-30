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

#include "events/EventSystem.h"
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
			Susi::Event event_one("heartbeat::one");
			Susi::Event event_five("heartbeat::five");
			Susi::Event event_ten("heartbeat::ten");
			Susi::Event event_minute("heartbeat::minute");
			Susi::Event event_five_minute("heartbeat::fiveMinute");
			while(!this->_stop){
				++count %= 300;
				Susi::publish(event_one);
				if(count % 5 == 0){
					Susi::publish(event_five);
				}
				if(count % 10 == 0){
					Susi::publish(event_ten);
				}
				if(count % 60 == 0){
					Susi::publish(event_minute);
				}
				if(count % 300 == 0){
					Susi::publish(event_five_minute);
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
