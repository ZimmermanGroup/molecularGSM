# Input arguments
message("SIMPLE_TPL_INSTALL_BASE = '${SIMPLE_TPL_INSTALL_BASE}'")
message("LIBDIR_NAME = '${LIBDIR_NAME}'")
message("OUTPUT_CMAKE_FRAG_FILE = '${OUTPUT_CMAKE_FRAG_FILE}'")

set(simpleTplLinkOptions
"set(TPL_ENABLE_SimpleTpl ON CACHE BOOL \"\")
set(TPL_SimpleTpl_INCLUDE_DIRS
  \"${SIMPLE_TPL_INSTALL_BASE}/include\"
  CACHE STRING \"\")
set(TPL_SimpleTpl_LIBRARIES
  \"-L${SIMPLE_TPL_INSTALL_BASE}/${LIBDIR_NAME}\"
  -lsimpletpl
  CACHE STRING \"\")
")

file(WRITE "${OUTPUT_CMAKE_FRAG_FILE}" "${simpleTplLinkOptions}")
