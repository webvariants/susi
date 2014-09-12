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

#ifndef __IO_CONTROLLER__
#define __IO_CONTROLLER__

#include <Poco/DirectoryIterator.h>
#include <Poco/Path.h>
#include <Poco/File.h>

#include <string>
#include <stdio.h>
#include <fstream>
#include <stdexcept>

#include "logger/Logger.h"

namespace Susi {

	class IOController {
	public:
		Poco::Path base_path;

		IOController();
		IOController(std::string base_path);
		// high level
		std::size_t writeFile(std::string filename,char* ptr, std::size_t len);
		std::size_t writeFile(std::string filename,std::string content);
		std::string readFile(std::string filename);
		bool movePath(std::string source_path, std::string dest_path); // copies the file (or directory) to the given path and removes the original file. The target path can be a directory.
		bool copyPath(std::string source_path, std::string dest_path); // copies the file (or directory) to the given path. The target path can be a directory.
		bool deletePath(std::string path); // delete file or directory(includes delete of subdirs)

		// low level
		//std::fstream open(std::string filename, std::string mode);
		bool makeDir(std::string dir);
		bool setExecutable(std::string path, bool isExecutable);
		bool getExecutable(std::string path);
		
		// helper functions
		Poco::Path getAbsPathFromString(std::string path);
		Poco::Path getAbsDirFromString(std::string path);
		bool checkDir(Poco::Path dir); // returns true if dir exists or false if dir not exists or is a file
		bool checkFile(Poco::Path dir); // returns true if file exists or false if file not exists

		bool checkFileExtension(std::string path, std::string file_extension);
	};
}

#endif // __IO_CONTROLLER__