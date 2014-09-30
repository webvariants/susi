/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 * 
 * @author: Thomas Krause (thomas.krause@webvariants.de)
 */

#ifndef __CONFIG__
#define __CONFIG__

#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>

#include <Poco/DirectoryIterator.h>
#include <Poco/Path.h>
#include <Poco/File.h>

class Config {
	public:
		Poco::Path base_path;

		bool kill_friendly = false;
		int  restart_trys = -1; // infinit
		bool isExecutable = false;
		std::map<std::string,std::string> _knownCommandLineOptions;

		Config();

		bool set(std::string key, std::string value);
		std::vector<std::string> & split(const std::string &s, char delim, std::vector<std::string> &elems);
		void registerCommandLineOption(std::string name, std::string key);
		std::vector<std::string> parseCommandLine(std::vector<std::string> argv);

		bool getExecutable(std::string path);
		Poco::Path getAbsPathFromString(std::string path);

		void printArgs(std::vector<std::string> argv_vec);

};

#endif // __CONFIG__
