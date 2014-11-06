/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 *
 * @author: Thomas Krause (thomas.krause@webvariants.de), Tino Rusch (tino.rusch@webvariants.de)
 */

#ifndef __SYS_CALL_COMPONENT__
#define __SYS_CALL_COMPONENT__

#include "world/BaseComponent.h"
#include "Poco/Foundation.h"

#include "syscall/SyscallWorker.h"
#include "logger/easylogging++.h"

namespace Susi {
    namespace Syscall {

        using Susi::Util::Any;
        using Susi::Events::EventPtr;

        class SyscallComponent : public Susi::System::BaseComponent {

        public:

            SyscallComponent( Susi::System::ComponentManager * mgr, Any::Object _commands, size_t threads = 4, size_t queuelen = 16 ) :
                Susi::System::BaseComponent {mgr},
                 commands {_commands},
            pool {threads,queuelen} {}

            virtual void start() override {
                subscribe( "syscall::exec", [this]( EventPtr evt ) {
                    handleExec( std::move( evt ) );
                } );
                LOG(INFO) <<  "started SyscallComponent" ;
            }

            virtual void stop() override {
                unsubscribeAll();
            }

            ~SyscallComponent() {
                stop();
                LOG(INFO) <<  "stopped SyscallComponent" ;
            }

        protected:
            Any::Object commands;
            Susi::Util::ThreadPool pool;

            void handleExec( EventPtr event ) {
                LOG(DEBUG) <<  "entering syscall::exec handler" ;
                std::string commandSpecifier = event->payload["cmd"];
                std::string commandline = commands[commandSpecifier];
                Any args = event->payload["args"];

                if( args.isObject() ) {
                    Any::Object argsObject = args;
                    for( auto& kv : argsObject ) {
                        std::string value = kv.second;
                        replaceString( commandline, "$" + kv.first,value );
                    }
                }
                std::vector<std::string> arguments;
                std::string binary {"noshell"};
#if defined(POCO_OS_FAMILY_UNIX)
                binary = "sh";
                arguments.push_back( "-c" );
#elif defined(POCO_OS_FAMILY_WINDOWS)
                binary = "cmd";
                arguments.push_back( "/C" );
#endif
                arguments.push_back( commandline );
                pool.add( Worker {std::move( event ),binary,arguments} );
            }

            void replaceString( std::string& subject, const std::string& search, const std::string& replace ) {
                size_t pos = 0;
                while( ( pos = subject.find( search, pos ) ) != std::string::npos ) {
                    subject.replace( pos, search.length(), replace );
                    pos += replace.length();
                }
            }
        };
    }
}

#endif // __SYS_CALL_COMPONENT__
