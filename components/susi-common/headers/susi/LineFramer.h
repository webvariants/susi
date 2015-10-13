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

#ifndef __JSONFRAMER__
#define __JSONFRAMER__

#include <map>
#include <sstream>
#include <functional>
#include "bson/Value.h"

#include <iostream>

namespace Susi {
    class LineFramer {
    protected:
        std::string message = "";
        size_t opening = 0;
        size_t closing = 0;
        bool inQuotes = false;
        std::function<void( std::string& )> _onMessage;
    public:
        LineFramer( std::function<void( std::string& )> onMessage );
        LineFramer();
        void operator=( const LineFramer & other );
        void collect( char * data, size_t len );
    };
}

#endif // __JSONFRAMER__
