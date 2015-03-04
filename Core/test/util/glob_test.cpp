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
#include "susi/util/Glob.h"

#include <iostream>

using namespace Susi::Util;

TEST(Glob, IsGlob) {

	EXPECT_FALSE(Glob::isGlob("hallo"));

	EXPECT_TRUE(Glob::isGlob("bl?h"));
	EXPECT_TRUE(Glob::isGlob("foo*"));
}

TEST(Glob, Match) {

	Glob glob("bl?h.*");

	EXPECT_FALSE(glob.match("hallo"));	

	EXPECT_TRUE(glob.match("blah.jpg"));
	EXPECT_TRUE(glob.match("blhh.jpg"));
	EXPECT_TRUE(glob.match("blah.png"));

	Glob glob2("*.*");

	EXPECT_FALSE(glob2.match("hallo"));	

	EXPECT_TRUE(glob2.match("foo.bar"));
	EXPECT_TRUE(glob2.match("foo.bar.baz"));

	Glob glob3("???"); // :-))
	
	EXPECT_FALSE(glob3.match("ab"));
	EXPECT_FALSE(glob3.match("abcd"));

	EXPECT_TRUE(glob3.match("xyz"));


	Glob glob4("abc*def");

	EXPECT_FALSE(glob4.match("abcoooooooooodefllll"));

	EXPECT_TRUE(glob4.match("abcoooooooooodef"));	
}