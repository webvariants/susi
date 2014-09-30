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

#include "db/DBComponent.h"

void Susi::DB::DBComponent::handleQuery( Susi::Events::EventPtr  event ) {
    try {
        std::string identifier = event->payload["identifier"];
        std::string query = event->payload["query"];

        auto db = getDatabase( identifier );
        if( db==nullptr ) {
            event->payload["success"] = false;
            throw std::runtime_error {"cant find db"};
        }
        else {
            auto res = db->query( query );
            event->payload["result"] = res;
        }

        event->payload["success"] = true;

    }
    catch( const std::exception & e ) {
        event->payload["success"] = false;

        std::string msg = "Error in handleQuery(): ";
        msg += e.what();
        throw std::runtime_error( msg );
    }
}