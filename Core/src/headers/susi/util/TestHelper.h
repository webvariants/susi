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

#ifndef __TEST_HELPER__
#define __TEST_HELPER__

#include <iostream>

namespace Susi {

    namespace Util {
        std::string __createRandomString( std::size_t len );
    }
}

#endif // __TEST_HELPER__