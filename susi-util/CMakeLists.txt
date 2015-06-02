
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

project(susi-util)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/headers )

if(WITH_SSL)
	set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall --std=c++11 -DWITH_SSL" )
	
	file( GLOB SRCFILES ${CMAKE_CURRENT_SOURCE_DIR}/sources/main.cpp)
	
	add_executable( ${PROJECT_NAME} ${SRCFILES})
	target_link_libraries(${PROJECT_NAME} susi-cpp)
else(WITH_SSL)
	set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall --std=c++11" )	
	
	file( GLOB SRCFILES ${CMAKE_CURRENT_SOURCE_DIR}/sources/TCPClient.cpp )
	
	add_executable( ${PROJECT_NAME} ${SRCFILES})
	target_link_libraries(${PROJECT_NAME} susi-cpp)	
endif(WITH_SSL)


install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION "/usr/local/bin")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/headers/ DESTINATION /usr/local/include)
