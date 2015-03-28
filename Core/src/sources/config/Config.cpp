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

#include "susi/config/Config.h"

void Susi::Config::loadConfig( std::string path ) {
    std::string content = "";
    BSON::Value configVar;

    if( !io.checkDir( path ) && !io.checkFile( path ) ) {
        std::string msg = "Susi::Config::loadConfig ";
        msg += ( "File: " + path + " file doesn't exist!" );
        //LOG(ERROR) <<  msg ;
        throw std::runtime_error( "file doesn't exist!" );
    }

    if( io.checkFile( path ) && io.checkFileExtension( path, file_extension ) ) {
        try {
            content = io.readFile( path );
        }
        catch( const std::exception & e ) {
            std::string msg = "Susi::Config::loadConfig ";
            msg += "Error reading Config File: " + path;
            msg += e.what();
            //LOG(ERROR) <<  msg ;
            throw std::runtime_error( msg );
        }

        try {
            configVar = BSON::Value::fromJSON( content );
        }
        catch( const std::exception & e ) {
            std::string msg = "Susi::Config::loadConfig ";
            msg += ( "File: " + path + " file cant be parsed as json!" );
            msg += e.what();
            //LOG(ERROR) <<  msg ;

            throw std::runtime_error( msg );
        }

        if( configVar.getType() != BSON::OBJECT ) {
            std::string msg = "Susi::Config::loadConfig ";
            msg += ( "File: " + path + " file doesn't contain a (json) object" );
            //LOG(ERROR) <<  msg ;
            throw std::runtime_error( "file doesn't contain a (json) object" );
        }

        if( _configVar.isUndefined() ) {
            // first load or empty
            //LOG(DEBUG) << "first load of config";
            _configVar = configVar;
        }
        else {
            //merge vars
            //LOG(DEBUG) << "merge config";
            //mergeOptions( "", configVar );
            merge(_configVar,configVar);
        }

        /*std::string msg = "Susi::Config::loadConfig ";
        msg += ( "File: " + path );
        //LOG(INFO) <<  msg ;*/
        //LOG(DEBUG) << "loaded cfg file "<<path;
        load_count++;

    }
    else if( io.checkDir( path ) ) {
        rec_dir( path );
    }
}

void Susi::Config::rec_dir( const std::string & path )
{
    Poco::DirectoryIterator end;
    for( Poco::DirectoryIterator it( path ); it != end; ++it ) {
        if( !it->isDirectory() ) {
            loadConfig( it->path() );
        }
        else if( it->isDirectory() ) {
            rec_dir( it->path() );
        }
    }
}

void Susi::Config::setFileExtension( std::string _file_extension ) {
    file_extension = _file_extension;
}

int Susi::Config::getLoadCount() {
    return load_count;
}

void Susi::Config::setLoadCount( int _load_count ) {
    load_count = _load_count;
}

void Susi::Config::merge( BSON::Value & ref, BSON::Value & other ){
    if(ref.isObject() && other.isObject()){
        for(auto & kv : other){
            if(ref[kv.first].isObject()){
                merge(ref[kv.first],kv.second);
            }else{
                ref[kv.first] = kv.second;
            }
        }
    }else{
        ref = other;
    }
}

// used to set a value in the config object (should be used by parseCommandLine())
void Susi::Config::set( std::string key, BSON::Value value ) {
    //LOG(DEBUG) <<  "set "+key+" to "+value.toJSON() ;
    std::vector<std::string> elems;
    Susi::Util::Helpers::split( key, '.', elems );
    auto * current = &_configVar;

    for( size_t e=0; e<elems.size()-1; e++ ) {
        current = &( *current )[elems[e]];
        if( ( *current ).isUndefined() ) {
            *current = BSON::Object {};
        }
    }
    ( *current )[elems.back()] = value;
}

// register a commandline option which will be recognized while parsing
void Susi::Config::registerCommandLineOption( std::string name, std::string key ) {
    _knownCommandLineOptions[name] = key;
}

// parses the commandline
// loops though the args and check if it is in _knownCommandLineOptions
// if so, place it in the _configVar at the specified key
// should throw an error if unknown commandline options are supplied, but should parse everything else
void Susi::Config::parseCommandLine( std::vector<std::string> argv ) {
    std::map<std::string,std::string>::iterator it;

    int argc = argv.size();

    for( int i = 1; i < argc; ++i ) {
        std::string option = argv[i];
        if( option[0] == '-' ) {
            if( option[1] == '-' )option = option.substr( 2 );
            else option = option.substr( 1 );

            BSON::Value v;
            std::string key = option;

            // find '='
            std::vector<std::string> elems;
            Susi::Util::Helpers::split( key, '=', elems );
            if( elems.size() == 2 ) {
                key = elems[0];
                v   = BSON::Value::fromJSON( elems[1] );
                set( key,v );
            }
            else {
                it = _knownCommandLineOptions.find( key );
                // key found
                if( it != _knownCommandLineOptions.end() ) {
                    std::string key = it->second;

                    if( i < ( argc-1 ) && argv[i+1][0]!='-' ) {
                        std::string value = argv[( i+1 )];
                        v = BSON::Value::fromJSON( value );
                        i++;
                    }
                    else {
                        v = BSON::Object {};
                    }
                    set( key,v );
                }
                // key not found
                else {
                    if( i < ( argc-1 ) && argv[i+1][0]!='-' ) {
                        std::string value = argv[( i+1 )];
                        v = BSON::Value::fromJSON( value );
                        i++;
                    }
                    else {
                        v = BSON::Object {};
                    }
                    set( key,v );
                }
            }
        }
    }

}

// get a config variable.
// keys are in this format: "foo.bar.baz"
// -> if we have {foo:{bar:{baz:123}}} get("foo.bar.baz") should return Any{123};
// should throw if key doesn't exist
BSON::Value Susi::Config::get( std::string key ) {
    std::vector<std::string> elems;

    if( key.length() == 0 ) {
        return _configVar;
    }
    else {
        Susi::Util::Helpers::split( key, '.', elems );

        BSON::Value found = _configVar;
        BSON::Value next  = _configVar;
        for( size_t e=0; e<elems.size(); e++ )
        {
            std::string elem = elems[e];
            found = next[elem];
            next = found;
            if( found.getType() == BSON::UNDEFINED ) {
                throw std::runtime_error( "key doesn't exist!"+key );
            }

        }

        return found;
    }
}

BSON::Value Susi::Config::getConfig() {
    return _configVar;
}

// returns a help message which shows which options are available
std::string Susi::Config::getHelp() {
    std::string result;

    return "foo";
}
