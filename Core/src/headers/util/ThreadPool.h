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

#ifndef __THREADPOOL__
#define __THREADPOOL__

#include <Poco/Runnable.h>
#include <Poco/ThreadPool.h>
#include <functional>
#include <vector>
#include "util/Channel.h"

#include <iostream>

namespace Susi {
    namespace Util {

        class ThreadPool {
        protected:
            Poco::ThreadPool pool;
            
            class Work : public Poco::Runnable {
            protected:
                ThreadPool * pool;
                std::function<void()>               work;
                std::function<void( std::string )>  error;
            public:
                Work(ThreadPool * _pool, std::function<void()> _work, std::function<void( std::string )> _error) 
                    : pool{_pool}, work{_work}, error{_error} {}
                virtual void run() override {
                    try{
                        work();
                    }catch(const std::exception & e){
                        try{
                            error(e.what());
                        }catch(...){}
                    }
                    std::lock_guard<std::mutex> lock{pool->mutex};
                    for(auto it=pool->workers.begin(); it!=pool->workers.end(); ++it){
                        if(&(*it) == this){
                            pool->workers.erase(it);
                            return;
                        }
                    }
                }
            };

            std::mutex mutex;
            std::vector<Work> workers;

        public:
            ThreadPool() : ThreadPool {4,16} {};
            ThreadPool( size_t workers, size_t buffsize ){}
            void add( std::function<void()> work,
                      std::function<void( std::string )> error = std::function<void( std::string )> {} ){
            
                std::lock_guard<std::mutex> lock{mutex};
                Work worker{this,std::move(work),std::move(error)};
                workers.push_back(std::move(worker));               
                pool.start(workers[workers.size()-1]);
            }
            ~ThreadPool(){
                pool.stopAll();
            }
        };

    } //ns::Util
} //ns::Susi


#endif // __THREADPOOL__