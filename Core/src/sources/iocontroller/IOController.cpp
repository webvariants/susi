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

#include "iocontroller/IOController.h"

//Constructor
Susi::IOController::IOController() {
	// use current as base path
	this->base_path = Poco::Path(Poco::Path::current());
}

Susi::IOController::IOController(std::string base_path) {
	// init with base path
	this->base_path = Poco::Path(base_path);
}

 //high level
std::size_t Susi::IOController::writeFile(std::string filename,char* ptr, std::size_t len) {

	Poco::Path dir = this->getAbsDirFromString(filename);

	if(this->checkDir(dir)) {
		// write File
		std::ofstream s((char*)filename.c_str());
		s<<ptr;
		s.close();

		return len;
	} else {
		std::string msg = "WriteFile: Dir ";
		msg += dir.current();
		msg += " don't exists!";
		Susi::Logger::error(msg);
		throw std::runtime_error{"WriteFile: Dir don't exists!"+filename};
	}
}

std::size_t Susi::IOController::writeFile(std::string filename,std::string content) {
	return this->writeFile(filename, (char*)content.c_str(), content.size());
}

std::string Susi::IOController::readFile(std::string filename) {

	if(this->checkFile(this->getAbsPathFromString(filename))) {
		std::string result = "";
		char * buffer;
		int length = 0;
		std::ifstream s((char*)filename.c_str());

		if(s) {
			// get length of file:
	    	s.seekg (0, s.end);
			length = s.tellg();
		    s.seekg (0, s.beg);

	    	buffer = new char [length];
			// read data as a block:
		    s.read(buffer,length);

		    if (!s) {
		    	std::string msg = "error: only ";
				msg += std::to_string(s.gcount());
				msg += " could be read";
				Susi::Logger::error(msg);
		    }

		    s.close();

		    result += std::string(buffer,length);

		    if(buffer)delete[] buffer;

		    return result;
		}
		throw std::runtime_error{"ReadFile: can't read file!"};
	} else {
		std::string msg = "ReadFile: ";
		msg += filename;
		msg += " don't exists!";
		Susi::Logger::error(msg);
		throw std::runtime_error{"ReadFile: File don't exists!"};
	}
}

bool Susi::IOController::movePath(std::string source_path, std::string dest_path) {
	Poco::File sf(this->getAbsPathFromString(source_path));
	Poco::Path dp = this->getAbsPathFromString(dest_path);

	if(sf.exists()) {
		sf.moveTo(dp.toString());
		return true;
	} else {
		std::string msg = "movePath: SOURCE_PATH ";
		msg += source_path;
		msg += " don't exists!";
		Susi::Logger::error(msg);
		throw std::runtime_error{"movePath: SOURCE_PATH don't exist!"};
	}
}

bool Susi::IOController::copyPath(std::string source_path, std::string dest_path) {
	Poco::File sf(this->getAbsPathFromString(source_path));
	Poco::Path dp = this->getAbsPathFromString(dest_path);

	if(sf.exists()) {
		sf.copyTo(dp.toString());
		return true;
	} else {
		std::string msg = "copyPath: SOURCE_PATH ";
		msg += source_path;
		msg += " don't exists!";
		Susi::Logger::error(msg);
		throw std::runtime_error{"copyPath: SOURCE_PATH don't exist!"};
	}
}

bool Susi::IOController::deletePath(std::string path) {

	Poco::File f(this->getAbsPathFromString(path));

	if(f.exists()) {
		if(f.isFile() || f.isDirectory()) {
			if(f.isFile()) {
				f.remove(false);
				return true;
			} else {
				f.remove(true); // deletes recursive
				return true;
			}
		} else {
			std::string msg = "Delete: PATH ";
			msg += path;
			msg += " is no FILE or DIR! (maybe PATH is LINK or DEVICE)";
			Susi::Logger::error(msg);
			throw std::runtime_error{"Delete: PATH is no FILE or DIR! (maybe PATH is LINK or DEVICE)"};
		}
	} else {
		return false;
	}
}

// low level
bool Susi::IOController::makeDir(std::string dir) {

	Poco::Path p(true);

	p.append(this->base_path);
	p.append(dir);

	Poco::File f(p);
	f.createDirectories();

	return true;
}

bool Susi::IOController::setExecutable(std::string path, bool isExecutable) {
	Poco::File f(this->getAbsPathFromString(path));

	if(f.exists()) {
		f.setExecutable(isExecutable);
		return true;
	} else {
		std::string msg = "setExecutable: PATH ";
		msg += path;
		msg += " is no FILE or DIR!";
		Susi::Logger::error(msg);
		throw std::runtime_error{"setExecutable: PATH is no FILE or DIR!"};
	}
}

bool Susi::IOController::getExecutable(std::string path) {
	Poco::File f(this->getAbsPathFromString(path));

	if(f.exists()) {
		return f.canExecute();
	} else {
		std::string msg = "getExecutable: PATH ";
		msg += path;
		msg += " is no FILE or DIR!";
		Susi::Logger::error(msg);
		throw std::runtime_error{"getExecutable: PATH is no FILE or DIR!"};
	}
}

// helper function
Poco::Path Susi::IOController::getAbsPathFromString(std::string path) {
	Poco::Path p(path);
	if(p.isRelative())
		p.makeAbsolute(this->base_path);
	return p;
}

Poco::Path Susi::IOController::getAbsDirFromString(std::string path) {
	Poco::Path p = this->getAbsPathFromString(path);
	p.makeParent();
	return p;
}

bool Susi::IOController::checkDir(Poco::Path dir) {

	Poco::File f(dir);

	if(f.exists()) {
		if(f.isDirectory()) {
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

bool Susi::IOController::checkFile(Poco::Path dir) {
	Poco::File f(dir);

	if(f.exists()) {
		if(f.isFile()) {
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

bool Susi::IOController::checkFileExtension(std::string path, std::string file_extension) {

	if(path.length() >= file_extension.length() + 1) {
		std::string sub_str = path.substr(path.length() - file_extension.length() - 1);

		return (sub_str == ("." + file_extension));
	}

	return false;
}
