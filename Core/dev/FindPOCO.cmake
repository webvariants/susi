# - Find POCO
# Find the native POCO includes and libraries
# This module defines
#  POCO_INCLUDE_DIR, where to find Poco.h, etc.
#  POCO_LIBRARIES, libraries to link against to use POCO.
#  POCO_VERSION, version in CMake decimal version# format (e.g., 1.4.1.1)
#  POCO_FOUND, If false, do not try to use POCO.
#
# Note that if specifying <version> in find_package(), use the CMake format
# rather than the POCO format (i.e., use "1.4.2.1" instead of "1.4.2p1").


# Convert a single hex character (assumed lowercase) from _inHex
# into decimal and return it as _outDec
function( _hexDigitToDec _inHex _outDec )
    if( ${_inHex} STREQUAL "a" )
        set( result 10 )
    elseif( ${_inHex} STREQUAL "b" )
        set( result 11 )
    elseif( ${_inHex} STREQUAL "c" )
        set( result 12 )
    elseif( ${_inHex} STREQUAL "d" )
        set( result 13 )
    elseif( ${_inHex} STREQUAL "e" )
        set( result 14 )
    elseif( ${_inHex} STREQUAL "f" )
        set( result 15 )
    else()
        set( result ${_inHex} )
    endif()
    set( ${_outDec} ${result} PARENT_SCOPE )
endfunction()

# Convert 2 hex chars (a hex value in the range 0 to ff) from _inHex
# to decimal and return it in _outDec. _inHex is case-insensitive.
function( _hexByteToDec _inHex _outDec )
    if( NOT _inHex )
        set( ${_outDec} 0 PARENT_SCOPE )
        return()
    endif()
    string( LENGTH ${_inHex} len )
    if( ${len} GREATER 2 )
        message( SEND_ERROR "_hexByteToDec: Input hex string too long. ${_inHex}, length ${len}" )
        return()
    endif()
    
    string( TOLOWER ${_inHex} lowerHex )
    set( expr 0 )
    while( lowerHex )
        string( SUBSTRING ${lowerHex} 0 1 char )
        _hexDigitToDec( ${char} decChar )
        set( expr "${expr}*16+${decChar}" )
        string( SUBSTRING ${lowerHex} 1 -1 lowerHex )
    endwhile()
    math( EXPR result ${expr} )
    set( ${_outDec} ${result} PARENT_SCOPE )
endfunction()



# Find the main Poco header.
set( POCO_INCLUDE_DIR )
find_path( POCO_INCLUDE_DIR Poco/Poco.h )


# Set POCO_VERSION in format MM.mm.vv.pp, where each component is a decimal
# (not hexidecimal, as it is in the Poco version.h file) number.
set( POCO_VERSION )
if( POCO_INCLUDE_DIR )
    set( _versionFile "${POCO_INCLUDE_DIR}/Poco/Version.h" )
    if( NOT EXISTS ${_versionFile} )
        message( SEND_ERROR "Can't find ${_versionFile}" )
    else()
        file( READ "${_versionFile}" _versionContents )
        string( REGEX REPLACE ".*#define POCO_VERSION[ \t]+0x([0-9A-Za-z]+).*"
            "\\1" _versionHex ${_versionContents} )
        string( REGEX MATCHALL "[0-9A-Za-z][0-9A-Za-z]" _versionsHex ${_versionHex} )
        
        foreach( _verNum ${_versionsHex} )
            _hexByteToDec( ${_verNum} _dec )
            set( POCO_VERSION "${POCO_VERSION}.${_dec}" )
        endforeach()
        string( SUBSTRING ${POCO_VERSION} 1 -1 POCO_VERSION )
    endif()
endif()


# Get a list of requested Poco components. PocoFoundation is assumed as always requested.
set( _requestedComponents )
foreach( _component ${POCO_FIND_COMPONENTS})
    list( APPEND _requestedComponents ${_component} )
endforeach()
list( APPEND _requestedComponents "PocoFoundation" )
list( REMOVE_DUPLICATES _requestedComponents )

# Find each library.
set( POCO_LIBRARIES )
foreach( lib ${_requestedComponents} )
    find_library( POCO_${lib}_LIBRARY
        NAMES ${lib}
        PATH_SUFFIXES lib
    )
    if( NOT POCO_${lib}_LIBRARY )
        message( WARNING "Could not find Poco component library ${lib}" )
    endif()
    list( APPEND POCO_LIBRARIES ${POCO_${lib}_LIBRARY} )
endforeach()


# handle the QUIETLY and REQUIRED arguments and set POCO_FOUND to TRUE if 
# all listed variables are TRUE
include( FindPackageHandleStandardArgs )
FIND_PACKAGE_HANDLE_STANDARD_ARGS( POCO
    REQUIRED_VARS POCO_INCLUDE_DIR POCO_LIBRARIES
    VERSION_VAR POCO_VERSION
)


mark_as_advanced(
    POCO_INCLUDE_DIR
    POCO_LIBRARIES
    POCO_VERSION
)