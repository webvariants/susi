
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

project(susi-cpp)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/headers )

if(WITH_SSL)
	set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall --std=c++11 -DWITH_SSL" )
	
	file( GLOB SRCFILES ${CMAKE_CURRENT_SOURCE_DIR}/sources/TCPClient.cpp
			       ${CMAKE_CURRENT_SOURCE_DIR}/sources/SSLClient.cpp 
			       ${CMAKE_CURRENT_SOURCE_DIR}/sources/SusiClient.cpp 
			       ${CMAKE_CURRENT_SOURCE_DIR}/sources/EventManager.cpp )
	
	file( GLOB HEADERFILES ${CMAKE_CURRENT_SOURCE_DIR}/headers/TCPClient.h )
	
	add_library( ${PROJECT_NAME} SHARED ${SRCFILES})
	target_link_libraries(${PROJECT_NAME}
		${CMAKE_THREAD_LIBS_INIT}
		${Boost_LIBRARIES}
		${OPENSSL_LIBRARIES}
		bson
		susi-common)


	add_executable( ${PROJECT_NAME}_test ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp)
	target_link_libraries(${PROJECT_NAME}_test
		${CMAKE_THREAD_LIBS_INIT}
		${Boost_LIBRARIES}
		${OPENSSL_LIBRARIES}
		bson
		susi-common
		susi-cpp)


else(WITH_SSL)
	set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall --std=c++11" )	
	
	file( GLOB SRCFILES ${CMAKE_CURRENT_SOURCE_DIR}/sources/TCPClient.cpp )
	
	file( GLOB HEADERFILES ${CMAKE_CURRENT_SOURCE_DIR}/headers/TCPClient.h )
	
	add_library( ${PROJECT_NAME} SHARED ${SRCFILES})
	target_link_libraries(${PROJECT_NAME}
		${CMAKE_THREAD_LIBS_INIT}
		${Boost_LIBRARIES}
		bson
		susi-common)	
endif(WITH_SSL)

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION "/usr/local/lib")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/headers/ DESTINATION /usr/local/include)
