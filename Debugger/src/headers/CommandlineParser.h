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

#ifndef __COMMANDLINE_PARSER__
#define __COMMANDLINE_PARSER__

#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>

namespace Debugger {
	class Parser {
		public:
			std::map<std::string,std::string> _knownCommandLineOptions;
			std::map<std::string,std::string> _knownCommandLineValues;
			std::map<std::string,std::string> _knownCommandLineType;


			bool set(std::string key, std::string value);
			std::string getValueByKey(std::string key);

			std::vector<std::string> & split(const std::string &s, char delim, std::vector<std::string> &elems);
			void registerCommandLineOption(std::string name, std::string key, std::string _default="", std::string _type="single");
			std::vector<std::string> parseCommandLine(std::vector<std::string> argv);

			void printParsedArgs();
	};
}

#endif // __COMMANDLINE_PARSER__
