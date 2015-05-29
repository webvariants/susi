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

#ifndef __SUSICLIENT__
#define __SUSICLIENT__

#include "susi/FramingClient.h"
#include "susi/JSONFramer.h"

#include <iostream>

#ifdef WITH_SSL
    #include "SSLClient.h"
    #define Client SSLClient
#else
    #include "TCPClient.h"
    #define Client TCPClient
#endif

namespace Susi {
    class SusiClient : public FramingClient<JSONFramer,Client> {
    public:
        SusiClient(std::string host, short port) : FramingClient<JSONFramer,Client>{host,port} {}
        #ifdef WITH_SSL
            SusiClient(std::string host, short port, std::string keyFile, std::string certificateFile) :
                FramingClient<JSONFramer,Client>{host,port,keyFile,certificateFile} {}
        #endif

        virtual void onFrame(std::string & frame) override {
            std::cout<<"onFrame: "<<frame;
        }


        virtual ~SusiClient() {}

    };

}

#endif // __SUSICLIENT__
