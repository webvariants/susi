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

#ifndef __SESSIONAWARECOMPONENT__
#define __SESSIONAWARECOMPONENT__

#include "world/BaseComponent.h"

namespace Susi {
	namespace System {
		class SessionAwareComponent : public BaseComponent {
		protected:
			std::shared_ptr<Susi::Sessions::SessionManagerComponent> sessionManager;
		public:
            SessionAwareComponent( ComponentManager * manager ) : BaseComponent{manager} {
                sessionManager = componentManager->getComponent<Susi::Sessions::SessionManagerComponent>( "sessionmanager" );
            }
            
            void assertAuthlevel(Susi::Events::Event & event, char authlevel){
                std::string & sessionID = event.sessionID;
                auto sessionAuthlevel = sessionManager->getSessionAttribute(sessionID,"authlevel");
                if(std::stoi(sessionAuthlevel) > authlevel){
                    throw std::runtime_error{"insufficient authlevel"};
            	}
        	}
		};
	}
}

#endif // __SESSIONAWARECOMPONENT__
