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

#ifndef __CONFIG__
#define __CONFIG__

#include <sstream>
#include <iostream>
#include "bson/Value.h"


#include "susi/util/Helpers.h"
#include "susi/iocontroller/IOController.h"
#include "susi/logger/easylogging++.h"


namespace Susi {


    class Config {
    protected:
        Susi::IOController io;

        // file extension
        std::string file_extension = "json";
        int load_count = 0;

        // holds the config
        BSON::Value _configVar;

        // holds infos to all possible commandline options
        std::map<std::string,std::string> _knownCommandLineOptions;

        // used to set a value in the config object (should be used by parseCommandLine())
        void set( std::string key, BSON::Value value );

        // recursive config loading, in alphabetical order
        void rec_dir( const std::string & path );
    public:
        Config() {};
        // constructs new config object
        // reads filename and parses into _configVar
        // should throw an error if
        //     - filename does not exist
        //     - file cant be parsed as json
        //     - file doesn't contain a (json) object
        Config( std::string filename ) {
            loadConfig( filename );
        }

        Config( const char *filename ) : Config {std::string{filename}} {}

        Config( BSON::Value cfg ) {
            _configVar = cfg;
        }

        void loadConfig( std::string path );

        void merge( BSON::Value & ref, BSON::Value & other );

        // register a commandline option which will be recognized while parsing
        void registerCommandLineOption( std::string name, std::string key );

        // parses the commandline
        // loops though the args and check if it is in _knownCommandLineOptions
        // if so, place it in the _configVar at the specified key
        // should throw an error if unknown commandline options are supplied, but should parse everything else
        void parseCommandLine( std::vector<std::string> argv );

        // get a config variable.
        // keys are in this format: "foo.bar.baz"
        // -> if we have {foo:{bar:{baz:123}}} get("foo.bar.baz") should return Any{123};
        // should throw if key doesn't exist
        BSON::Value get( std::string key );

        BSON::Value getConfig();

        // returns a help message which shows which options are available
        std::string getHelp();

        void setFileExtension( std::string _file_extension );

        int getLoadCount();
        void setLoadCount( int _load_count );
    };

}

#endif // __CONFIG__