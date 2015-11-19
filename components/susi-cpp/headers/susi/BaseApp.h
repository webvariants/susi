/*
 * Copyright (c) 2015, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 *
 * @author: Tino Rusch (tino.rusch@webvariants.de)
 */

#ifndef __BASEAPP__
#define __BASEAPP__

#include "susi/SusiClient.h"
#include <boost/program_options.hpp>
#include <fstream>
namespace po = boost::program_options;

namespace Susi {
    class BaseApp {
      protected:
        //override this to add dependencies
        std::string _dependencies = "susi-core";

        //commandline specific stuff
        std::string _progName;
        po::variables_map _vm;
        po::options_description _desc{"Allowed options"};

        //your command line options
        std::string _configPath;

        //the parsed config file
        BSON::Value _config;

        //the global susi-client
        std::shared_ptr<Susi::SusiClient> _susi;

        void showHelp(){
            std::cout << "usage: " << _progName << std::endl;
            std::cout << _desc << std::endl;
            exit(0);
        }

        void showDependencies(){
            std::cout << _dependencies << std::endl;
            exit(0);
        }

        void parseCommandLine(int argc, char **argv){
            _progName = argv[0];
            _desc.add_options()
                ("help,h", "show help message")
                ("dependencies,d","show dependencies")
                ("conf,c", po::value<std::string>(&_configPath)->default_value("sample-config.json"), "location of your config file");
            po::store(po::parse_command_line(argc, argv, _desc), _vm);
            po::notify(_vm);
        }

        void readConfigFile(){
            //read config file
            std::ifstream t(_configPath);
            if(!t){
                throw std::runtime_error{"can't open config file"};
            }
            std::string configStr((std::istreambuf_iterator<char>(t)),
                                  std::istreambuf_iterator<char>());
            _config = BSON::Value::fromJSON(configStr);
        }

        void startupSusi(){
            if(!_config["susi-addr"].isString() ||
               !_config["susi-port"].isInt64() ||
               !_config["cert"].isString() ||
               !_config["key"].isString()){
                throw std::runtime_error{"invalid config (need 'susi-addr', 'susi-port', 'cert' and 'key' entries to startup a susi client)"};
            }
            auto addr = _config["susi-addr"].getString();
            short port = static_cast<short>(_config["susi-port"].getInt64());
            auto key = _config["key"].getString();
            auto cert = _config["cert"].getString();
            _susi.reset(new Susi::SusiClient{addr,port,key,cert});
        }

      public:
        BaseApp(int argc, char **argv) {
            parseCommandLine(argc,argv);
            if(_vm.count("help")){
                showHelp();
            }
            if(_vm.count("dependencies")){
                showDependencies();
            }
            readConfigFile();
            startupSusi();
        }

        virtual ~BaseApp() {
            join();
        }

        virtual void start() = 0;

        void join(){
            _susi->join();
        }
    };

}

#endif // __BASEAPP__
