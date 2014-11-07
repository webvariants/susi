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

#include "world/ComponentManager.h"
#include "logger/easylogging++.h"
/*#include "zlib.h"*/
#include "lzma.h"

namespace Susi {

void rolloutHandler(const char* filename, std::size_t size);
bool fileExists(const std::string & filename);
/*bool compress_zlib(std::string infile, std::string outfile);*/
bool compress_lzma(std::string infile, std::string outfile);

class LoggerComponent : public Susi::System::Component {
public:
    LoggerComponent(){}

    virtual void start() override {
        el::Loggers::addFlag(el::LoggingFlag::StrictLogFileSizeCheck);
        el::Helpers::installPreRollOutCallback(rolloutHandler);
        LOG(INFO) <<  "started LoggerComponent" ;
    }

    virtual void stop() override {}

    ~LoggerComponent() {
        stop();
        LOG(INFO) <<  "stopped LoggerComponent" ;
    }
};

}


#endif // __LOGGER__
