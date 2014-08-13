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

#include "webstack/HttpServer.h"
#include "events/EventSystem.h"
#include "sessions/SessionManager.h"
#include "db/Database.h"
#include "pluginloader/PluginLoader.h"
#include "sessions/SessionManagerEventInterface.h"
#include "tiny-js/JSEngine.h"

#include "world/World.h"

#include <Poco/Net/HTTPServerParams.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/Dynamic/Struct.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/Util/AbstractConfiguration.h>
#include <thread>
#include <vector>

using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;
using Poco::Util::AbstractConfiguration;
using Poco::Util::OptionCallback;

class SusiApp : public Poco::Util::ServerApplication {
protected:
	bool _helpRequested = false;

	int main(const std::vector<std::string> &){
		if(!_helpRequested){

			setupDefaultProperties();

			world.setup();

			Susi::subscribe("*",[](Susi::Event & event){
				Susi::debug("Event: "+event.toString());
			});

			Susi::Event startEvent("global::start");
			Susi::publish(startEvent);

			waitForTerminationRequest();  // wait for CTRL-C or kill
			world.tearDown();
			std::cout << std::endl << "Shutting down..." << std::endl;
		}
		return Application::EXIT_OK;
	}

	void setupDefaultProperties(){
		auto & cfg = config();
		cfg.setInt("session.lifetime",cfg.getInt("session.lifetime",1200));
		cfg.setInt("session.interval",cfg.getInt("session.interval",10));
		cfg.setInt("logger.level",cfg.getInt("logger.level",Susi::Logger::WARN | Susi::Logger::ERROR | Susi::Logger::INFO));
		cfg.setString("webstack.addr",cfg.getString("webstack.addr","[::1]:8080"));
		cfg.setString("tcpserver.addr",cfg.getString("tcpserver.addr","[::1]:4000"));
		cfg.setString("webstack.assets",cfg.getString("webstack.assets","./assets/"));
		cfg.setString("jsengine.source",cfg.getString("jsengine.source","./controller.js"));
		cfg.setString("enginestarter.root",cfg.getString("enginestarter.root","./controller/"));
		cfg.setString("db.config",cfg.getString("db.config","./dbs.conf"));
	}

	void defineOptions(OptionSet& options)
	{
		Application::defineOptions(options);

		options.addOption(
			Option("help", "h", "display help information on command line arguments")
				.required(false)
				.repeatable(false)
				.callback(OptionCallback<SusiApp>(this, &SusiApp::handleHelp)));

		options.addOption(
			Option("define", "D", "define a configuration property")
				.required(false)
				.repeatable(true)
				.argument("name=value")
				.callback(OptionCallback<SusiApp>(this, &SusiApp::handleDefine)));

		options.addOption(
			Option("config-file", "f", "load configuration data from a file")
				.required(false)
				.repeatable(true)
				.argument("file")
				.callback(OptionCallback<SusiApp>(this, &SusiApp::handleConfig)));

		options.addOption(
			Option("print", "p", "print configuration data ")
				.required(false)
				.repeatable(false)
				.callback(OptionCallback<SusiApp>(this, &SusiApp::handlePrint)));
	}

	void handleHelp(const std::string& name, const std::string& value){
		_helpRequested = true;
		displayHelp();
	}

	void handleDefine(const std::string& name, const std::string& value){
		defineProperty(value);
	}

	void handleConfig(const std::string& name, const std::string& value){
		Susi::debug("load config "+value);
		loadConfiguration(value);
	}

	void handlePrint(const std::string& name, const std::string& value){
		setupDefaultProperties();
		printProperties("");
	}

	void displayHelp(){
		HelpFormatter helpFormatter(options());
		helpFormatter.setCommand(commandName());
		helpFormatter.setUsage("OPTIONS");
		helpFormatter.setHeader("SUSI - a Universal System Interface");
		helpFormatter.format(std::cout);
	}

	void defineProperty(const std::string& def){
		std::string name;
		std::string value;
		std::string::size_type pos = def.find('=');
		if (pos != std::string::npos)
		{
			name.assign(def, 0, pos);
			value.assign(def, pos + 1, def.length() - pos);
		}
		else name = def;
		config().setString(name, value);
	}

	void printProperties(const std::string& base)
	{
		AbstractConfiguration::Keys keys;
		config().keys(base, keys);
		if (keys.empty())
		{
			if (config().hasProperty(base))
			{
				std::string msg;
				msg.append(base);
				msg.append(" = ");
				msg.append(config().getString(base));
				logger().information(msg);
			}
		}
		else
		{
			for (AbstractConfiguration::Keys::const_iterator it = keys.begin(); it != keys.end(); ++it)
			{
				std::string fullKey = base;
				if (!fullKey.empty()) fullKey += '.';
				fullKey.append(*it);
				printProperties(fullKey);
			}
		}
	}


};

int main(int argc, char** argv){
	SusiApp app;
	return app.run(argc, argv);
}
