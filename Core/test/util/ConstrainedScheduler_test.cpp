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

#include "gtest/gtest.h"
#include "util/ConstrainedScheduler.h"

TEST(ConstrainedScheduler,Basic){
	Susi::Util::ConstrainedScheduler scheduler;

	std::set<std::string> tasks{"a","b","c"};
	scheduler.addConstraint("c","b");
	auto res = scheduler.schedule(tasks);

	EXPECT_EQ("a",res[0]);
	EXPECT_EQ("c",res[1]);
	EXPECT_EQ("b",res[2]);

	//add impossible constraint
	scheduler.addConstraint("b","c");
	EXPECT_THROW({
		scheduler.schedule(tasks);
	},std::runtime_error);
}
