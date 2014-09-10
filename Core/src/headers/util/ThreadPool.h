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

#include <thread>
#include <functional>
#include <vector>
#include "util/Channel.h"

#include <iostream>

namespace Susi {
namespace Util {

class ThreadPool {
protected:
	struct Work {
		std::function<void()> 				work;
		std::function<void(std::string)> 	error;
	};

	Channel<Work> _workChannel;
	std::vector<std::thread> _threads;
	
	void startThread();

public:
	ThreadPool(size_t workers, size_t buffsize);
	void add(std::function<void()> work, 
			 std::function<void(std::string)> error = std::function<void(std::string)>{});
	~ThreadPool();
};
	
} //ns::Util
} //ns::Susi


#endif // __THREADPOOL__