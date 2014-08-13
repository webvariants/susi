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

#ifndef __GLOB__
#define __GLOB__

#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdexcept>


namespace Susi {

	class Glob {
	public:		
		Glob(std::string str);		
	};
}

#endif // __GLOB__