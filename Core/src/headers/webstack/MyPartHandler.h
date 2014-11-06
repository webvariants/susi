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

#ifndef __MY_PART_HANDLER__
#define __MY_PART_HANDLER__

#include "Poco/Net/PartHandler.h"
#include "Poco/Net/MessageHeader.h"

#include "Poco/CountingStream.h"
#include "Poco/StreamCopier.h"

#include "Poco/FileStream.h"
#include "Poco/File.h"


namespace Susi {

    class MyPartHandler: public Poco::Net::PartHandler {
    public:
        MyPartHandler( std::string uploadDirectory ): _length( 0 ), _uploadDirectory( uploadDirectory ) {};

        void handlePart( const Poco::Net::MessageHeader& header, std::istream& stream )
        {
            _type = header.get( "Content-Type", "(unspecified)" );
            if( header.has( "Content-Disposition" ) )
            {
                std::string disp;
                Poco::Net::NameValueCollection params;
                Poco::Net::MessageHeader::splitParameters( header["Content-Disposition"], disp, params );
                _name = params.get( "name", "(unnamed)" );
                _fileName = params.get( "filename", "(unnamed)" );
            }

            LOG(DEBUG) <<  "in part handler: name: "+_fileName ;

            Poco::CountingInputStream istr( stream );

            if( _fileName != "" ) {
                LOG(DEBUG) <<  "target filepath: "+_uploadDirectory + _fileName ;
                Poco::FileOutputStream fos( _uploadDirectory + _fileName, std::ios::binary );
                Poco::StreamCopier::copyStream( istr, fos );
                fos.close();
            }
            _length = istr.chars();
        }

        int length() const {
            return _length;
        }

        const std::string& name() const {
            return _name;
        }
        const std::string& fileName() const {
            return _fileName;
        }
        const std::string& contentType() const {
            return _type;
        }

    private:
        int _length;
        std::string _uploadDirectory;
        std::string _type;
        std::string _name;
        std::string _fileName;
    };

}

#endif // __FORM_REQUEST_HANDLER__