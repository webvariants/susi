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

#ifndef __LOGGER__
#define __LOGGER__

#include <iostream>
#include <Poco/LocalDateTime.h>
#include <Poco/DateTimeFormatter.h>
#include <Poco/DateTimeFormat.h>

namespace Susi {

    extern unsigned char logLevel;

    class Logger {
    protected:
        static void log( std::string message,unsigned char level ) {
            if( level & logLevel ) {
                Poco::LocalDateTime now;
                std::string prefix = "";
                if( level & INFO ) prefix = "INFO";
                if( level & WARN ) prefix = "WARN";
                if( level & ERROR ) prefix = "ERROR";
                if( level & DEBUG ) prefix = "DEBUG";
                std::string date = Poco::DateTimeFormatter::format( now,Poco::DateTimeFormat::HTTP_FORMAT );
                std::cerr<<prefix<<" : "<<date<<" : "<<message<<std::endl;
            }
        }
    public:
        static const unsigned char NOTHING = 0;
        static const unsigned char INFO = 1;
        static const unsigned char WARN = 2;
        static const unsigned char ERROR = 4;
        static const unsigned char DEBUG = 8;
        static const unsigned char ALL = INFO | WARN | ERROR | DEBUG;

        static void log( std::string msg ) {
            info( msg );
        }
        static void info( std::string msg ) {
            log( msg,INFO );
        }
        static void warn( std::string msg ) {
            log( msg,WARN );
        }
        static void debug( std::string msg ) {
            log( msg,DEBUG );
        }
        static void error( std::string msg ) {
            log( msg,ERROR );
        }
        static void setLevel( unsigned char level ) {
            logLevel = level;
        }
    };


}

#endif // __LOGGER__