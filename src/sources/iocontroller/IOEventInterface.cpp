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
#include "world/World.h"

void Susi::IOEventInterface::initEventInterface() {	
	subscribe("io::writeFile", handleWriteFile);
	subscribe("io::readFile", handleReadFile);
	subscribe("io::deletePath", handleDeletePath);
	subscribe("io::movePath", handleMovePath);
	subscribe("io::copyPath", handleCopyPath);

	subscribe("io::makeDir", handleMakeDir);
	subscribe("io::setExecutable", handleSetExecutable);
	subscribe("io::getExecutable", handleGetExecutable);
}

void Susi::IOEventInterface::handleWriteFile(Event & event) {
	try{
		std::string filename = event.payload["filename"];
		std::string content  = event.payload["content"];
		world.ioController->writeFile(filename, content);
		answerEvent(event, true);
	}catch(const std::exception & e){
		std::string msg = "Error in handleWriteFile(): ";
		msg += e.what();
		Susi::error(msg);
		answerEvent(event, false);
	}
}

void Susi::IOEventInterface::handleReadFile(Event & event) {
	try{
		std::string filename = event.payload["filename"];
		answerEvent(event, world.ioController->readFile(filename));
	}catch(const std::exception & e){
		std::string msg = "Error in handleReadFile(): ";
		msg += e.what();
		Susi::error(msg);
		answerEvent(event, false);
	}
}

void Susi::IOEventInterface::handleDeletePath(Event & event) {
	try{
		std::string path = event.payload["path"];
		answerEvent(event, world.ioController->deletePath(path));
	}catch(const std::exception & e){
		std::string msg = "Error in handleDeleteFile(): ";
		msg += e.what();
		Susi::error(msg);
		answerEvent(event, false);
	}
}

void Susi::IOEventInterface::handleCopyPath(Event & event) {
	try{
		std::string source_path = event.payload["source_path"];
		std::string dest_path = event.payload["dest_path"];
		answerEvent(event, world.ioController->copyPath(source_path, dest_path));
	}catch(const std::exception & e){
		std::string msg = "Error in handleCopyPath(): ";
		msg += e.what();
		Susi::error(msg);
		answerEvent(event, false);
	}
}

void Susi::IOEventInterface::handleMovePath(Event & event) {
	try{
		std::string source_path = event.payload["source_path"];
		std::string dest_path = event.payload["dest_path"];
		answerEvent(event, world.ioController->movePath(source_path, dest_path));
	}catch(const std::exception & e){
		std::string msg = "Error in handleMovePath(): ";
		msg += e.what();
		Susi::error(msg);
		answerEvent(event, false);
	}
}

void Susi::IOEventInterface::handleMakeDir(Event & event) {
	try{
		std::string dir = event.payload["dir"];
		answerEvent(event, world.ioController->makeDir(dir));
	}catch(const std::exception & e){
		std::string msg = "Error in handleMakeDir(): ";
		msg += e.what();
		Susi::error(msg);
		answerEvent(event, false);
	}
}

void Susi::IOEventInterface::handleSetExecutable(Event & event) {
	try{
		std::string path = event.payload["path"];
		bool isExecutable = event.payload["isExecutable"];
		answerEvent(event, world.ioController->setExecutable(path, isExecutable));
	}catch(const std::exception & e){
		std::string msg = "Error in handleSetExecutable(): ";
		msg += e.what();
		Susi::error(msg);
		answerEvent(event, false);
	}
}

void Susi::IOEventInterface::handleGetExecutable(Event & event) {
	try{
		std::string path = event.payload["path"];
		answerEvent(event, world.ioController->getExecutable(path));
	}catch(const std::exception & e){
		std::string msg = "Error in handleGetExecutable(): ";
		msg += e.what();
		Susi::error(msg);
		answerEvent(event, false);
	}
}