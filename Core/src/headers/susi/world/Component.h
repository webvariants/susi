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

#ifndef __COMPONENT__
#define __COMPONENT__

#include "bson/Value.h"

namespace Susi {
    namespace System {

        class Component {
        public:
            virtual void start() = 0;
            virtual void stop() = 0;
        };

    }
}

#endif // __COMPONENT__
