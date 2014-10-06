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

#ifndef __IO_CONTROLLER_COMPONENT__
#define __IO_CONTROLLER_COMPONENT__

#include "world/BaseComponent.h"
#include "iocontroller/IOController.h"

namespace Susi {
    class IOControllerComponent : public Susi::System::BaseComponent , public IOController {
    public:
        IOControllerComponent( Susi::System::ComponentManager * mgr ) :
            Susi::System::BaseComponent {mgr}, IOController {} {}

        ~IOControllerComponent() {
            stop();
            Susi::Logger::info( "stopped IOControllerComponent" );
        }

        virtual void start() override {
            subscribe( std::string{"io::writeFile"}, Susi::Events::Processor{[this]( ::Susi::Events::EventPtr evt ) {
                handleWriteFile( std::move( evt ) );
            }} );
            subscribe( std::string{"io::readFile"}, Susi::Events::Processor{[this]( ::Susi::Events::EventPtr evt ) {
                handleReadFile( std::move( evt ) );
            }} );
            subscribe( std::string{"io::deletePath"},Susi::Events::Processor{[this]( ::Susi::Events::EventPtr evt ) {
                handleDeletePath( std::move( evt ) );
            }} );
            subscribe( std::string{"io::movePath"}, Susi::Events::Processor{[this]( ::Susi::Events::EventPtr evt ) {
                handleMovePath( std::move( evt ) );
            }} );
            subscribe( std::string{"io::copyPath"}, Susi::Events::Processor{[this]( ::Susi::Events::EventPtr evt ) {
                handleCopyPath( std::move( evt ) );
            }} );
            subscribe( std::string{"io::makeDir"}, Susi::Events::Processor{[this]( ::Susi::Events::EventPtr evt ) {
                handleMakeDir( std::move( evt ) );
            }} );
            subscribe( std::string{"io::setExecutable"}, Susi::Events::Processor{[this]( ::Susi::Events::EventPtr evt ) {
                handleSetExecutable( std::move( evt ) );
            }} );
            subscribe( std::string{"io::getExecutable"}, Susi::Events::Processor{[this]( ::Susi::Events::EventPtr evt ) {
                handleGetExecutable( std::move( evt ) );
            }} );
            Susi::Logger::info( "started IOControllerComponent" );
        }

        virtual void stop() override {
            unsubscribeAll();
        }
    protected:
        void handleWriteFile( Susi::Events::EventPtr event );
        void handleReadFile( Susi::Events::EventPtr event );
        void handleDeletePath( Susi::Events::EventPtr event );
        void handleCopyPath( Susi::Events::EventPtr event );
        void handleMovePath( Susi::Events::EventPtr event );
        void handleMakeDir( Susi::Events::EventPtr event );
        void handleSetExecutable( Susi::Events::EventPtr event );
        void handleGetExecutable( Susi::Events::EventPtr event );
    };
}

#endif // __IO_CONTROLLER_COMPONENT__

