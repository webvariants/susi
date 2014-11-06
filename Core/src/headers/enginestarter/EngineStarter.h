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

#ifndef __ENGINE_STARTER__
#define __ENGINE_STARTER__

#include <Poco/DirectoryIterator.h>
#include <Poco/Process.h>
#include <vector>
#include <string>

#include <chrono>
#include <thread>

#include "logger/easylogging++.h"

namespace Susi {
    namespace EngineStarter {
        class Starter {
            std::vector<Poco::ProcessHandle> phs;
        public:
            Starter();
            void execute( std::string path );
            void killall();
        private:
            void rec_dir( const std::string & path );
        };
    }
}

#endif // __ENGINE_STARTER__