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

#include "iocontroller/IOControllerComponent.h"

void Susi::IOControllerComponent::handleWriteFile(Susi::Events::EventPtr event) {
	try{
		std::string filename     = event->payload["filename"];
		std::string content      = event->payload["content"];
		std::size_t bytesWritten = writeFile(filename, content);
		event->payload["success"] = true;
		event->payload["bytesWritten"] = static_cast<int>(bytesWritten);
	}catch(const std::exception & e){
		event->payload["success"] = false;
		std::string msg = "Error in handleWriteFile(): ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}

void Susi::IOControllerComponent::handleReadFile(Susi::Events::EventPtr event) {
	try{
		std::string filename = event->payload["filename"];
		event->payload["content"] = readFile(filename);
		event->payload["success"] = true;
	}catch(const std::exception & e){
		event->payload["success"] = false;
		std::string msg = "Error in handleReadFile(): ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}

void Susi::IOControllerComponent::handleDeletePath(Susi::Events::EventPtr event) {
	try{
		std::string path = event->payload["path"];
		event->payload["success"] = deletePath(path);
	}catch(const std::exception & e){
		event->payload["success"] = false;
		std::string msg = "Error in handleDeleteFile(): ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}

void Susi::IOControllerComponent::handleCopyPath(Susi::Events::EventPtr event) {
	try{
		std::string source_path = event->payload["source_path"];
		std::string dest_path = event->payload["dest_path"];
		event->payload["success"] = copyPath(source_path, dest_path);
	}catch(const std::exception & e){
		event->payload["success"] = false;
		std::string msg = "Error in handleCopyPath(): ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}

void Susi::IOControllerComponent::handleMovePath(Susi::Events::EventPtr event) {
	try{
		std::string source_path = event->payload["source_path"];
		std::string dest_path = event->payload["dest_path"];
		event->payload["success"] = movePath(source_path, dest_path);
	}catch(const std::exception & e){
		event->payload["success"] = false;
		std::string msg = "Error in handleMovePath(): ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}

void Susi::IOControllerComponent::handleMakeDir(Susi::Events::EventPtr event) {
	try{
		std::string dir = event->payload["dir"];
		
		event->payload["success"] = makeDir(dir);
	}catch(const std::exception & e){
		event->payload["success"] = false;
		std::string msg = "Error in handleMakeDir(): ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}

void Susi::IOControllerComponent::handleSetExecutable(Susi::Events::EventPtr event) {
	try{
		std::string path = event->payload["path"];
		bool isExecutable = event->payload["isExecutable"];
		event->payload["success"] = setExecutable(path, isExecutable);
	}catch(const std::exception & e){
		event->payload["success"] = false;
		std::string msg = "Error in handleSetExecutable(): ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}

void Susi::IOControllerComponent::handleGetExecutable(Susi::Events::EventPtr event) {
	try{
		std::string path = event->payload["path"];
		event->payload["success"] = getExecutable(path);
	}catch(const std::exception & e){
		event->payload["success"] = false;
		std::string msg = "Error in handleGetExecutable(): ";
		msg += e.what();
		throw std::runtime_error(msg);
	}
}