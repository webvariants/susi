#include "enginestarter/EngineStarter.h"

Susi::EngineStarter::Starter::Starter() {}

void Susi::EngineStarter::Starter::killall() {
    while( !phs.empty() )
    {
        auto & ph =phs.back();
        Poco::Process::kill( ph );
        phs.pop_back();
    }
}

void Susi::EngineStarter::Starter::execute( std::string path ) {
    this->rec_dir( path );
}

void Susi::EngineStarter::Starter::rec_dir( const std::string & path )
{
    Poco::DirectoryIterator end;
    for( Poco::DirectoryIterator it( path ); it != end; ++it ) {
        if( !it->isDirectory() && it->canExecute() ) {
            std::vector<std::string> args;
            Poco::ProcessHandle ph = Poco::Process::launch( it->path(), args );
            this->phs.push_back( ph );
        }
        else if( it->isDirectory() ) {
            rec_dir( it->path() );
        }
    }
}