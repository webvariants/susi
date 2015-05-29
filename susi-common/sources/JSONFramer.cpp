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

#include "susi/JSONFramer.h" 

Susi::JSONFramer::JSONFramer( std::function<void( std::string& )> onMessage ) : _onMessage {onMessage} {}
Susi::JSONFramer::JSONFramer() {}

void Susi::JSONFramer::operator=( const JSONFramer & other ) {
    _onMessage = other._onMessage;
};

void Susi::JSONFramer::collect( char * data, size_t len ) {
    for( size_t i=0; i<len; i++ ) {
        char c = data[i];
        message += c;
        if( c=='"' && i>0 && data[i-1] != '\\'){
            inQuotes = !inQuotes;
        }
        if( c=='{' && !inQuotes)opening++;
        if( c=='}' && !inQuotes) {
            closing++;
            if( opening==closing ) {
                if( _onMessage ) {
                    _onMessage( message );
                }
                else {
                    std::cout<<"no function!"<<std::endl;
                }
                message = "";
                opening=0;
                closing=0;
            }
        }
    }
}
