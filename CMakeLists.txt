# Declare minimum required CMake version before anything to avoid problems.
SET(GSM_TRIBITS_CMAKE_MINIMUM_REQUIRED 2.8.11)
CMAKE_MINIMUM_REQUIRED(VERSION ${GSM_TRIBITS_CMAKE_MINIMUM_REQUIRED})

# Define project name and set up major options
INCLUDE(${CMAKE_SOURCE_DIR}/ProjectName.cmake)

#Indicate to TriBITS that we might use Python
SET(${PROJECT_NAME}_USES_PYTHON TRUE)

set(TARGET_NAME "gsm" CACHE STRING "Description")

PROJECT(${PROJECT_NAME} NONE)
SET(${PROJECT_NAME}_ENABLE_gsm ON CACHE BOOL "")
SET(TPL_ENABLE_MPI OFF )

#Pull in TriBITS for the project
SET(${PROJECT_NAME}_TRIBITS_DIR
"${CMAKE_CURRENT_SOURCE_DIR}/cmake/tribits"
  CACHE PATH "" )
INCLUDE (${${PROJECT_NAME}_TRIBITS_DIR}/TriBITS.cmake)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GIT_SHA1)

#Process the TriBITS Project
TRIBITS_PROJECT()

ADD_SUBDIRECTORY(TEST)
ADD_SUBDIRECTORY(docs)
