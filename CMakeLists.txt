# Declare minimum required CMake version before anything to avoid problems.
CMAKE_MINIMUM_REQUIRED(VERSION 3.15)

# Define project name and set up major options
INCLUDE(${CMAKE_SOURCE_DIR}/ProjectName.cmake)

ADD_SUBDIRECTORY(GSM)
#ADD_SUBDIRECTORY(TEST)
