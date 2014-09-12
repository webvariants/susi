###################################################################################
 # Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 #
 # This file is released under the terms of the MIT license. You can find he
 # complete text in the attached LICENSE file or online at:
 #
 # http://www.opensource.org/licenses/mit-license.php
 # 
 # @author: Thomas Krause (thomas.krause@webvariants.de)
 ###################################################################################

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project( "SUSI-TINYJSENGINE" )

set( CMAKE_COLOR_MAKEFILE ON)

set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g -Wall --std=c++11 -fPIC -O3" )

include_directories( ${CMAKE_SOURCE_DIR}/../../Core/src/headers )
include_directories( ${CMAKE_SOURCE_DIR}/src)
include_directories( ${CMAKE_SOURCE_DIR}/test/gtest-1.7.0/include )
include_directories( /usr/local/include/soci )
include_directories( /usr/local/include/Poco )
include_directories( /usr/local/include )

file( GLOB SRCFILES ${CMAKE_SOURCE_DIR}/src/*.cpp )

add_executable( susi-tinyjsengine ${SRCFILES} ${CMAKE_SOURCE_DIR}/TinyJSEngine_main.cpp)
target_link_libraries(susi-tinyjsengine ${CMAKE_SOURCE_DIR}/../../Core/build/libsusi.so)
target_link_libraries(susi-tinyjsengine PocoFoundation)
target_link_libraries(susi-tinyjsengine PocoUtil)
target_link_libraries(susi-tinyjsengine PocoJSON)
target_link_libraries(susi-tinyjsengine PocoNet)
target_link_libraries(susi-tinyjsengine soci_core)
target_link_libraries(susi-tinyjsengine soci_sqlite3)
target_link_libraries(susi-tinyjsengine soci_firebird)
target_link_libraries(susi-tinyjsengine pthread)

file( GLOB TESTFILES ${CMAKE_SOURCE_DIR}/test/*.cpp )

add_executable( susi-tinyjsengine-tests ${SRCFILES} ${TESTFILES} )
target_link_libraries(susi-tinyjsengine-tests ${CMAKE_SOURCE_DIR}/../../Core/build/libsusi.so)
target_link_libraries(susi-tinyjsengine-tests PocoFoundation)
target_link_libraries(susi-tinyjsengine-tests PocoUtil)
target_link_libraries(susi-tinyjsengine-tests PocoJSON)
target_link_libraries(susi-tinyjsengine-tests PocoNet)
target_link_libraries(susi-tinyjsengine-tests soci_core)
target_link_libraries(susi-tinyjsengine-tests soci_sqlite3)
target_link_libraries(susi-tinyjsengine-tests soci_firebird)
target_link_libraries(susi-tinyjsengine-tests pthread)

target_link_libraries(susi-tinyjsengine-tests ${CMAKE_SOURCE_DIR}/test/gtest-1.7.0/lib/.libs/libgtest.so.0)
target_link_libraries(susi-tinyjsengine-tests ${CMAKE_SOURCE_DIR}/test/gtest-1.7.0/lib/.libs/libgtest_main.so.0)
