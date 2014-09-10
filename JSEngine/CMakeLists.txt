###################################################################################
#
#  Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
#
#  This file is released under the terms of the MIT license. You can find the
#  complete text in the attached LICENSE file or online at:
#
#  http://www.opensource.org/licenses/mit-license.php
#
#  @author: Tino Rusch (tino.rusch@webvariants.de), Christian Sonderfeld (christian.sonderfeld@webvariants.de)
#
###################################################################################

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project( "SUSI-JSENGINE" )

set( CMAKE_COLOR_MAKEFILE ON)

set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g -Wall --std=c++11 -fPIC -O3" )

include_directories( ${CMAKE_SOURCE_DIR}/../susi/src/headers )
include_directories( /usr/local/include/soci )
include_directories( /usr/local/include/Poco )
include_directories( /usr/local/include )
include_directories( /opt/v8/include )

file( GLOB SRCFILES ${CMAKE_SOURCE_DIR}/*.cpp )

add_executable( susi-jsengine ${SRCFILES} )
target_link_libraries(susi-jsengine ${CMAKE_SOURCE_DIR}/../susi/build/libsusi.so)
target_link_libraries(susi-jsengine PocoFoundation)
target_link_libraries(susi-jsengine PocoUtil)
target_link_libraries(susi-jsengine PocoJSON)
target_link_libraries(susi-jsengine PocoNet)
target_link_libraries(susi-jsengine soci_core)
target_link_libraries(susi-jsengine soci_sqlite3)
target_link_libraries(susi-jsengine soci_firebird)
target_link_libraries(susi-jsengine pthread)
target_link_libraries(susi-jsengine /opt/v8/out/x64.release/lib.target/libv8.so)
target_link_libraries(susi-jsengine /opt/v8/out/x64.release/lib.target/libicuuc.so)
target_link_libraries(susi-jsengine /opt/v8/out/x64.release/lib.target/libicui18n.so)


