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

#ifndef __SYS_CALL_WORKER__
#define __SYS_CALL_WORKER__

#include "Poco/Runnable.h"
#include "Poco/Thread.h"
#include "Poco/Process.h"
#include "Poco/PipeStream.h"
#include "Poco/StreamCopier.h"

//#include "events/EventSystem.h"
#include "events/global.h" 
#include "util/Any.h"
#include <fstream>
#include <iostream>

namespace Susi {
	namespace Syscall {
		class Worker : public Poco::Runnable {
			public:
				Worker(std::string process_type, std::string cmd, std::vector<std::string> args, bool bg);

				std::string getPipeContent(Poco::PipeInputStream & str);

				virtual void run();
			private:
				std::string _process_type;
				std::string _cmd;
				std::vector<std::string> _args;
				bool _bg;
				
		};
	}
}

#endif // __SYS_CALL_WORKER__