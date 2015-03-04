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


#include <syslog.h>
#include <lzma.h>

#include "susi/world/ComponentManager.h"
#include "susi/logger/easylogging++.h"

namespace Susi {

void rolloutHandler(const char* filename, std::size_t size);
bool fileExists(const std::string & filename);
/*bool compress_zlib(std::string infile, std::string outfile);*/
bool compress_lzma(std::string infile, std::string outfile);

class LoggerComponent : public Susi::System::Component {
protected:
    std::string _configFile;
    bool _syslog;

    class SyslogBuilder : public el::base::DefaultLogBuilder {
    protected:
    public:
        el::base::type::string_t build(const el::LogMessage* logMessage, bool appendNewLine) const {
            el::base::type::string_t msg = el::base::DefaultLogBuilder::build(logMessage,appendNewLine);
            if(logMessage->level() == el::Level::Info){
                syslog(LOG_INFO,"%s",msg.c_str());
            }else if(logMessage->level() == el::Level::Debug){
                syslog(LOG_DEBUG,"%s",msg.c_str());
            }else if(logMessage->level() == el::Level::Error){
                syslog(LOG_ERR,"%s",msg.c_str());
            }
            return msg;
        }
    };

public:
    LoggerComponent(std::string cfg, bool syslog) : _configFile{cfg},_syslog{syslog} {}

    virtual void start() override {
        if(_configFile!=""){
            el::Configurations conf(_configFile);
            el::Loggers::reconfigureAllLoggers(conf);
        }
        if(_syslog){
            ELPP_INITIALIZE_SYSLOG("susi", LOG_PID | LOG_CONS | LOG_PERROR, LOG_USER);
            el::LogBuilderPtr syslogBuilder = el::LogBuilderPtr(new SyslogBuilder());
            el::Loggers::getLogger("default")->setLogBuilder(syslogBuilder);
        }
        el::Loggers::addFlag(el::LoggingFlag::StrictLogFileSizeCheck);
        el::Helpers::installPreRollOutCallback(rolloutHandler);
        LOG(INFO) <<  "started LoggerComponent with syslog "<<_syslog ;
    }

    virtual void stop() override {}

    ~LoggerComponent() {
        stop();
        LOG(INFO) <<  "stopped LoggerComponent" ;
    }
};

}


#endif // __LOGGER__
