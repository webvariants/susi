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

#ifndef __IO_CONTROLLER_COMPONENT__
#define __IO_CONTROLLER_COMPONENT__

#include "world/BaseComponent.h"
#include "iocontroller/IOController.h"

namespace Susi {
	class IOControllerComponent : public Susi::System::BaseComponent , public IOController {
		public:
			IOControllerComponent (Susi::System::ComponentManager * mgr, std::string base_path) :
				Susi::System::BaseComponent{mgr}, IOController{base_path} {}

			~IOControllerComponent() {
				stop();
			}

			virtual void start() override {
				subscribe("io::writeFile", [this](::Susi::Events::EventPtr evt){handleWriteFile(std::move(evt));});
				subscribe("io::readFile", [this](::Susi::Events::EventPtr evt){handleReadFile(std::move(evt));});
				subscribe("io::deletePath",[this](::Susi::Events::EventPtr evt){handleDeletePath(std::move(evt));});
				subscribe("io::movePath", [this](::Susi::Events::EventPtr evt){handleMovePath(std::move(evt));});
				subscribe("io::copyPath", [this](::Susi::Events::EventPtr evt){handleCopyPath(std::move(evt));});
				subscribe("io::makeDir", [this](::Susi::Events::EventPtr evt){handleMakeDir(std::move(evt));});
				subscribe("io::setExecutable", [this](::Susi::Events::EventPtr evt){handleSetExecutable(std::move(evt));});
				subscribe("io::getExecutable", [this](::Susi::Events::EventPtr evt){handleGetExecutable(std::move(evt));});
			}

			virtual void stop() override {
				unsubscribeAll();
			}
		protected:
			void handleWriteFile(Susi::Events::EventPtr event) {
				try{
					std::string filename     = event->payload["filename"];
					std::string content      = event->payload["content"];
					std::size_t bytesWritten = writeFile(filename, content);
					event->payload["bytesWritten"] = static_cast<int>(bytesWritten);
				}catch(const std::exception & e){
					std::string msg = "Error in handleWriteFile(): ";
					msg += e.what();
					throw std::runtime_error(msg);
				}
			}

			void handleReadFile(Susi::Events::EventPtr event) {
				try{
					std::string filename = event->payload["filename"];
					event->payload["content"] = readFile(filename);
				}catch(const std::exception & e){
					std::string msg = "Error in handleReadFile(): ";
					msg += e.what();
					throw std::runtime_error(msg);
				}
			}

			void handleDeletePath(Susi::Events::EventPtr event) {
				try{
					std::string path = event->payload["path"];
					event->payload["success"] = deletePath(path);
				}catch(const std::exception & e){
					std::string msg = "Error in handleDeleteFile(): ";
					msg += e.what();
					throw std::runtime_error(msg);
				}
			}

			void handleCopyPath(Susi::Events::EventPtr event) {
				try{
					std::string source_path = event->payload["source_path"];
					std::string dest_path = event->payload["dest_path"];
					event->payload["success"] = copyPath(source_path, dest_path);
				}catch(const std::exception & e){
					std::string msg = "Error in handleCopyPath(): ";
					msg += e.what();
					throw std::runtime_error(msg);
				}
			}

			void handleMovePath(Susi::Events::EventPtr event) {
				try{
					std::string source_path = event->payload["source_path"];
					std::string dest_path = event->payload["dest_path"];
					event->payload["success"] = movePath(source_path, dest_path);
				}catch(const std::exception & e){
					std::string msg = "Error in handleMovePath(): ";
					msg += e.what();
					throw std::runtime_error(msg);
				}
			}

			void handleMakeDir(Susi::Events::EventPtr event) {
				try{
					std::string dir = event->payload["dir"];
					event->payload["success"] = makeDir(dir);
				}catch(const std::exception & e){
					std::string msg = "Error in handleMakeDir(): ";
					msg += e.what();
					throw std::runtime_error(msg);
				}
			}

			void handleSetExecutable(Susi::Events::EventPtr event) {
				try{
					std::string path = event->payload["path"];
					bool isExecutable = event->payload["isExecutable"];
					event->payload["success"] = setExecutable(path, isExecutable);
				}catch(const std::exception & e){
					std::string msg = "Error in handleSetExecutable(): ";
					msg += e.what();
					throw std::runtime_error(msg);
				}
			}

			void handleGetExecutable(Susi::Events::EventPtr event) {
				try{
					std::string path = event->payload["path"];
					event->payload["success"] = getExecutable(path);
				}catch(const std::exception & e){
					std::string msg = "Error in handleGetExecutable(): ";
					msg += e.what();
					throw std::runtime_error(msg);
				}
			}

	};
}

#endif // __IO_CONTROLLER_COMPONENT__

