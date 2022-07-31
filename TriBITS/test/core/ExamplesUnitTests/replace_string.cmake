# CMake -P script to replace a string in a text file with another string.
#
# Usage:
#
#   cmake -D FILE=<file> -D STRING_TO_REPLACE="<str-to-replace>" \
#     -D REPLACEMENT_STRING="<replacement-string" \
#     -P replace_string.cmake
#

# Echo input args
message("FILE = '${FILE}'")
message("STRING_TO_REPLACE = '${STRING_TO_REPLACE}'")
message("REPLACEMENT_STRING = '${REPLACEMENT_STRING}'")

file(READ "${FILE}" fileContentsStr)
string(REPLACE "${STRING_TO_REPLACE}" "${REPLACEMENT_STRING}"
  fileContentsStr "${fileContentsStr}")
file(WRITE "${FILE}" "${fileContentsStr}")
