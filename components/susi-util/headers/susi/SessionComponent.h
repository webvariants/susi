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

#ifndef __BASECOMPONENT__
#define __BASECOMPONENT__

#include "susi/BaseComponent.h"

namespace Susi {
    class SessionStateComponent : protected Susi::BaseComponent {
    public:
        SessionComponent(Susi::SusiClient & client) : BaseComponent{client} {}
    protected:
        std::map<int,std::map<std::string,BSON::Value>> sessionStates_;

        void handleSetEvent(BSON::Value & event){
            if(event["payload"].isObject() && 
               event["payload"]["id"].isInteger() && 
               event["payload"]["key"].isString()){
                
            }
        }

        void handleGetEvent(BSON::Value & event){

        }

        void handleSessionNewEvent(const BSON::Value & event);
        void handleSessionLostEvent(const BSON::Value & event);   

    };

}

#endif // __BASECOMPONENT__
