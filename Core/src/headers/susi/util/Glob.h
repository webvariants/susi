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

#ifndef __GLOB__
#define __GLOB__

#include <string>

namespace Susi {
    namespace Util {
        class Glob {
        protected:
            std::string _pattern;
        public:
            Glob( std::string pattern );
            bool match( std::string str );

            static bool isGlob( std::string pattern );
        };
    }
}

#endif // __GLOB__