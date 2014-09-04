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

#ifndef __HEARTBEATCOMPONENT__
#define __HEARTBEATCOMPONENT__

#include <thread>
#include <atomic>
#include "world/BaseComponent.h"

namespace Susi {
	class HeartBeatComponent : public Susi::System::BaseComponent {
	protected:
		std::atomic<bool> stop;
		std::thread t;
	public:
		HeartBeatComponent(Susi::System::ComponentManager * mgr) :
			Susi::System::BaseComponent{mgr},
			stop{false} {}

		virtual start() override {
			t = std::move(std::thread{
				[this](){
					int count = 0;
					std::chrono::seconds interval(1);
					while(!this->stop.load()){
						++count %= 300;
						publish(createEvent("heartbeat::one"));
						if(count % 5 == 0){
							publish(createEvent("heartbeat::five"));
						}
						if(count % 10 == 0){
							publish(createEvent("heartbeat::tem"));
						}
						if(count % 60 == 0){
							publish(createEvent("heartbeat::minute"));
						}
						if(count % 300 == 0){
							publish(createEvent("heartbeat::fiveMinute"));
						}
						std::this_thread::sleep_for(interval);
					}
				}
			});
		}

		virtual stop() override {
			stop.store(true);
			if(t.joinable())t.join();
		}

		virtual ~HeartBeat(){
			stop();
		}
	};
}

#endif // __HEARTBEATCOMPONENT__

