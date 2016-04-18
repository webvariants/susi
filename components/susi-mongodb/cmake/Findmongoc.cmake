# Try to find mongoc.

# TODO: currently this is just a shim around pkg-config. We will have to do
# something for windows eventually.

# These vars will be defined:
# MONGOC_FOUND - System has mongoc
# MONGOC_VERSION - The version of mongoc we found
# MONGOC_INCLUDE_DIRS - The mongoc include directories
# MONGOC_LIBRARY_DIRS - The mongoc library directories
# MONGOC_LIBRARIES - The libraries needed to use mongoc
# MONGOC_DEFINITIONS - The compiler switches required for using mongoc

find_package(PkgConfig)
pkg_check_modules(MONGOC QUIET libmongoc-1.0)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(MONGOC
    REQUIRED_VARS
      MONGOC_FOUND
      MONGOC_INCLUDE_DIRS
      MONGOC_LIBRARY_DIRS
      MONGOC_LIBRARIES
    VERSION_VAR
      MONGOC_VERSION
    FAIL_MESSAGE
      "mongoc is not installed on this system!"
)
