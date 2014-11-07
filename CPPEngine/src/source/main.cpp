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

#include <iostream>
#include <mutex>
#include <chrono>
#include <condition_variable>

#include "engine/Engine.h"

#include "logger/Logger.h"

int main(int argc, char** argv){

	auto logger = Susi::LoggerComponent{};
	logger.start();

	std::shared_ptr<Susi::Cpp::Engine> e{new Susi::Cpp::Engine{}};
	e->populateController();
	e->start();
	e->getApi()->publish(std::move(e->getApi()->createEvent("samplecontroller::test")));

	std::condition_variable cond;
	std::mutex mutex;
	std::unique_lock<std::mutex> lock{mutex};
	cond.wait(lock, [](){return false;});
}
