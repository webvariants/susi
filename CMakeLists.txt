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

SET(CMAKE_COLOR_MAKEFILE ON)

SET( LIBS "-l PocoFoundation -l PocoUtil -l PocoJSON -l PocoNet -l soci_core -l soci_sqlite3 -l soci_firebird -l pthread")

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g -Wall --std=c++11 -fPIC -O3" )
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${LIBS}" )

include_directories( ${CMAKE_SOURCE_DIR}/src/headers )
include_directories( /usr/local/include/soci )

FILE( GLOB SRCFILES ${CMAKE_SOURCE_DIR}/src/sources/**/*.cpp )

add_executable( susi ${CMAKE_SOURCE_DIR}/src/sources/main.cc ${SRCFILES} ) 