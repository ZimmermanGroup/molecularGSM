########################################################################
# Setup for RPATH handling
########################################################################


if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
  set(SHARED_LIB_EXT "dylib")
  set(RPATH_INSPECT_CMND "otool")
  set(RPATH_INSPECT_ARG "-L")
elseif (CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
  set(SHARED_LIB_EXT "so")
  set(RPATH_INSPECT_CMND "objdump")
  set(RPATH_INSPECT_ARG "-x")
endif()
