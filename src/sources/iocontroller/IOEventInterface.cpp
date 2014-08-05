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

#include "iocontroller/IOEventInterface.h"
 
void Susi::IOEventInterface::initEventInterface() {	
	Susi::Events::subscribe("io::writeFile", handleWriteFile);	
	Susi::Events::subscribe("io::readFile", handleReadFile);
	Susi::Events::subscribe("io::deletePath", handleDeletePath);
	Susi::Events::subscribe("io::movePath", handleMovePath);
	Susi::Events::subscribe("io::copyPath", handleCopyPath);
	Susi::Events::subscribe("io::makeDir", handleMakeDir);
	Susi::Events::subscribe("io::setExecutable", handleSetExecutable);
	Susi::Events::subscribe("io::getExecutable", handleGetExecutable);
}

void Susi::IOEventInterface::handleWriteFile(Susi::Events::EventPtr event) {
	try{
		std::string filename     = event->payload["filename"];
		std::string content      = event->payload["content"];
		std::size_t bytesWritten = world.ioController->writeFile(filename, content);
		event->payload["bytesWritten"] = static_cast<int>(bytesWritten);
	}catch(const std::exception & e){
		std::string msg = "Error in handleWriteFile(): ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}

void Susi::IOEventInterface::handleReadFile(Susi::Events::EventPtr event) {
	try{
		std::string filename = event->payload["filename"];
		event->payload["content"] = world.ioController->readFile(filename);
	}catch(const std::exception & e){
		std::string msg = "Error in handleReadFile(): ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}

void Susi::IOEventInterface::handleDeletePath(Susi::Events::EventPtr event) {
	try{
		std::string path = event->payload["path"];
		event->payload["success"] = world.ioController->deletePath(path);		
	}catch(const std::exception & e){
		std::string msg = "Error in handleDeleteFile(): ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}

void Susi::IOEventInterface::handleCopyPath(Susi::Events::EventPtr event) {
	try{
		std::string source_path = event->payload["source_path"];
		std::string dest_path = event->payload["dest_path"];
		event->payload["success"] = world.ioController->copyPath(source_path, dest_path);		
	}catch(const std::exception & e){
		std::string msg = "Error in handleCopyPath(): ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}

void Susi::IOEventInterface::handleMovePath(Susi::Events::EventPtr event) {
	try{
		std::string source_path = event->payload["source_path"];
		std::string dest_path = event->payload["dest_path"];
		event->payload["success"] = world.ioController->movePath(source_path, dest_path);
	}catch(const std::exception & e){
		std::string msg = "Error in handleMovePath(): ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}

void Susi::IOEventInterface::handleMakeDir(Susi::Events::EventPtr event) {
	try{
		std::string dir = event->payload["dir"];
		event->payload["success"] = world.ioController->makeDir(dir);
	}catch(const std::exception & e){
		std::string msg = "Error in handleMakeDir(): ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}

void Susi::IOEventInterface::handleSetExecutable(Susi::Events::EventPtr event) {
	try{
		std::string path = event->payload["path"];
		bool isExecutable = event->payload["isExecutable"];
		event->payload["success"] = world.ioController->setExecutable(path, isExecutable);
	}catch(const std::exception & e){
		std::string msg = "Error in handleSetExecutable(): ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}

void Susi::IOEventInterface::handleGetExecutable(Susi::Events::EventPtr event) {
	try{
		std::string path = event->payload["path"];
		event->payload["success"] = world.ioController->getExecutable(path);
	}catch(const std::exception & e){
		std::string msg = "Error in handleGetExecutable(): ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}