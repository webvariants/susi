###################################################################################
#
#  Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
#
#  This file is released under the terms of the MIT license. You can find the
#  complete text in the attached LICENSE file or online at:
#
#  http://www.opensource.org/licenses/mit-license.php
#
#  @author: Tino Rusch (tino.rusch@webvariants.de)
#
###################################################################################

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project( "SUSI" )

set( CMAKE_COLOR_MAKEFILE ON)

set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g -Wall --std=c++11 -fPIC -O3" )

include_directories( ${CMAKE_SOURCE_DIR}/src/headers )
include_directories( ${CMAKE_SOURCE_DIR}/test/gtest-1.7.0/include )
include_directories( /usr/local/include/soci )
include_directories( /usr/local/include/Poco )
include_directories( /usr/local/include )

file( GLOB SRCFILES ${CMAKE_SOURCE_DIR}/src/sources/**/*.cpp )
file( GLOB TESTFILES ${CMAKE_SOURCE_DIR}/test/*/*.cpp )

add_library( libsusi SHARED ${SRCFILES} )
set_target_properties(libsusi PROPERTIES OUTPUT_NAME susi)

add_executable( susi ${CMAKE_SOURCE_DIR}/src/sources/main.cc )
target_link_libraries(susi libsusi)
target_link_libraries(susi PocoFoundation)
target_link_libraries(susi PocoUtil)
target_link_libraries(susi PocoJSON)
target_link_libraries(susi PocoNet)
target_link_libraries(susi soci_core)
target_link_libraries(susi soci_sqlite3)
target_link_libraries(susi soci_firebird)
target_link_libraries(susi pthread)

add_executable( susi_test ${TESTFILES})
target_link_libraries(susi_test libsusi)
target_link_libraries(susi_test PocoFoundation)
target_link_libraries(susi_test PocoUtil)
target_link_libraries(susi_test PocoJSON)
target_link_libraries(susi_test PocoNet)
target_link_libraries(susi_test soci_core)
target_link_libraries(susi_test soci_sqlite3)
target_link_libraries(susi_test soci_firebird)
target_link_libraries(susi_test pthread)
target_link_libraries(susi_test ${CMAKE_SOURCE_DIR}/test/gtest-1.7.0/lib/.libs/libgtest.so.0)
target_link_libraries(susi_test ${CMAKE_SOURCE_DIR}/test/gtest-1.7.0/lib/.libs/libgtest_main.so.0)
