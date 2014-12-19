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

#ifndef __IEVENTSYSTEM__
#define __IEVENTSYSTEM__

#include "events/Event.h"

namespace Susi {
namespace Events {

//Event pointer types
typedef std::unique_ptr<Event,std::function<void( Event* )>> EventPtr;
typedef std::shared_ptr<Event> SharedEventPtr;

//Callback types
typedef std::function<void( EventPtr )> Processor;
typedef std::function<void( SharedEventPtr )> Consumer;

//a event predicate
typedef std::function<bool( Event& )> Predicate;

class IEventSystem {
public:
    virtual long subscribe( std::string topic, Processor processor, std::string name="" ) = 0;
    //virtual long subscribe( Predicate pred, Processor processor, std::string name="" ) = 0;
    virtual long subscribe( std::string topic, Consumer consumer, std::string name="" ) = 0;
    //virtual long subscribe( Predicate pred, Consumer consumer, std::string name="" ) = 0;
    virtual bool unsubscribe( long id ) = 0;
    virtual void publish( EventPtr event, Consumer finishCallback = Consumer {}, bool processorsOnly = false, bool consumersOnly = false) = 0;
    virtual void ack( EventPtr event ) = 0;
    //virtual void addConstraint( std::pair<std::string,std::string> constraint ) = 0;
    virtual EventPtr createEvent( std::string topic ) = 0;
};

}
}

#endif // __IEVENTSYSTEM__
