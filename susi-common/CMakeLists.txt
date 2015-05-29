
###################################################################################
#
#  Copyright (c) 2015, webvariants GmbH, http://www.webvariants.de
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

project(susi-common)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/headers )

set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall --std=c++11 -Os -DWITH_SSL" )

file( GLOB SRCFILES ${CMAKE_CURRENT_SOURCE_DIR}/sources/JSONFramer.cpp)

file( GLOB HEADERFILES ${CMAKE_CURRENT_SOURCE_DIR}/headers/*.h )

add_library( ${PROJECT_NAME} SHARED ${SRCFILES})
target_link_libraries(${PROJECT_NAME}
	${CMAKE_THREAD_LIBS_INIT}
	${Boost_LIBRARIES}
	bson)

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION "/usr/local/lib")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/headers/ DESTINATION /usr/local/include)
