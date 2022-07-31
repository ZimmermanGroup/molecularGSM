# Copy files from a list of directories given a glob experession
#
# Usage::
#
#   cmake
#     -D FROM_DIRS="<dir1>;<dir2>;..."
#     -D GLOB_EXPR="<glob-expr>"
#     -D TO_DIR="<to-dir>"
#     -P copy_files_glob.cmake
#
#
# NOTE: Commas in 'FROM_DIR' will be replaced with ';'.
#

string(REPLACE "," ";" FROM_DIRS "${FROM_DIRS}")

foreach(dir ${FROM_DIRS})
  file(GLOB_RECURSE matchedFiles "${dir}/${GLOB_EXPR}")
  foreach(file ${matchedFiles})
    message("Coping file '${file}' to '${TO_DIR}'")
    file(COPY "${file}" DESTINATION "${TO_DIR}")
  endforeach()
endforeach()

