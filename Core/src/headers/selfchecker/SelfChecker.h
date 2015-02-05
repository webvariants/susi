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

#ifndef __SELFCHECKER__
#define __SELFCHECKER__


#include "world/ComponentManager.h"
#include "logger/easylogging++.h"

#include <cstdlib>


namespace Susi {

class SelfCheckerComponent : public Susi::System::Component {
protected:
    Susi::Util::Any             _config;
    std::string                 _failCommand;
    std::chrono::milliseconds   _timeout;
    std::chrono::milliseconds   _interval;

    std::atomic<bool> _stop;
    std::thread       _runloop;

public:
    SelfCheckerComponent(Susi::Util::Any config) : _config{config} {
        if(_config["failCommand"].isString()){
            _failCommand = _config["failCommand"];
        }else{
            _failCommand = "";
        }
        if(_config["timeout"].isInteger()){
            _timeout = _config["timeout"];
        }else{
            _timeout = 500;
        }
        if(_config["interval"].isInteger()){
            _interval = _config["interval"];
        }else{
            _interval = 5000;
        }
    }

    virtual void start() override {
        _runloop = std::move(std::thread{[this](){
            _stop.store(false);
            std::mutex mutex;
            std::condition_variable cond;
            bool b1,b2;
            subscribe(std::string{"selfchecker::check"},Susi::Events::Processor{[&c1,&b1](Susi::Events::EventPtr){
                b1 = true;
            }});
            while(!_stop.load()){
                Poco::Thread::sleep(_interval.count());
                b1 = b2 = false;
                auto event = createEvent("selfchecker::check");
                publish(std::move(event),[&c3,&b3](Susi::Events::SharedEventPtr){
                    b2 = true;
                    cond.notify_one();
                });
                {
                    std::unique_lock<std::mutex> lk(mutex);
                    c1.wait_for(lk,_timeout,[&b2](){return b2;});
                }
                if((!b1 || !b2) && _failCommand != ""){
                    std::system(_failCommand.c_str());
                }
            }
        }});
        LOG(INFO) <<  "started SelfCheckerComponent";
    }

    virtual void stop() override {
        _stop.store(true);
        if(_runloop.joinable())_runloop.join();
    }

    ~SelfCheckerComponent() {
        stop();
        LOG(INFO) <<  "stopped SelfCheckerComponent" ;
    }
};

}


#endif // __SELFCHECKER__
