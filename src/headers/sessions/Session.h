/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 *
 * @author: Christian Sonderfeld (christian.sonderfeld@webvariants.de)
 */

#include <map>
#include <Poco/Timestamp.h>
#include <Poco/Dynamic/Var.h>
#include <chrono>

namespace Susi {
	class Session {
	protected:
		Poco::Timestamp deadline;
		std::map<std::string, Poco::Dynamic::Var> attributes;
		std::map<std::string, std::vector<Poco::Dynamic::Var>> multiAttributes;

	public:
		Session() : Session(std::chrono::milliseconds(10000)) {}
		Session(std::chrono::milliseconds milliseconds);
		bool isDead();
		void addTime(std::chrono::milliseconds milliseconds);
		bool setAttribute(std::string key, Poco::Dynamic::Var value);
		void pushAttribute(std::string key, Poco::Dynamic::Var value);
		bool removeAttribute(std::string key);
		Poco::Dynamic::Var getAttribute(std::string key);
		bool die();
	};
}
