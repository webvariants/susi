/*
 * Copyright (c) 2015, webvariants GmbH, http://www.webvariants.de
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

#include <deque>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace Susi{
    class ThreadPool {
    public:
        ThreadPool(size_t numWorkers = 1){
            stop_.store(false);
            for(size_t i=0;i<numWorkers;i++){
                workers_.emplace_back([this](){
                    while(!stop_.load()){
                        Work work;
                        {
                            std::unique_lock<std::mutex> lock( mutex_ );
                            workAvailable_.wait( lock,[this]() {
                                return workQueue_.size() > 0 || stop_.load()==false;
                            });
                            if(!stop_.load()){
                                work = std::move(workQueue_[0]);
                                workQueue_.pop_front();
                            }
                        }
                        if(work.work){
                            try{
                                work.work();
                            }catch(const std::exception & e){
                                if(work.error){
                                    work.error(e.what());
                                }
                            }
                        }
                    }
                });
            }
        }

        void add(std::function<void()> work, std::function<void(const std::string &)> error =  std::function<void(const std::string &)>{}){
            std::unique_lock<std::mutex> lock( mutex_ );                
            workQueue_.emplace_back(Work{std::move(work),std::move(error)});
            workAvailable_.notify_one();
        }

    protected:
        struct Work {
            std::function<void()> work;
            std::function<void(const std::string &)> error;
        };
        std::atomic<bool> stop_;
        std::mutex mutex_;
        std::condition_variable workAvailable_;
        std::deque<Work> workQueue_;
        std::vector<std::thread> workers_;
    };
}

#endif // __THREADPOOL__