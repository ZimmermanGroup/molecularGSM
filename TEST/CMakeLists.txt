MESSAGE(STATUS "exe = gfstringq${${PROJECT_NAME}_CMAKE_EXECUTABLE_SUFFIX}")
MESSAGE(STATUS "location = ${PROJECT_BINARY_DIR}/${PROJECT_NAME}")
MESSAGE(STATUS "test location = ${CMAKE_CURRENT_BINARY_DIR}")
MESSAGE(STATUS "!!!!Mina TEST = ${DIRNAME}${ROOT_NAME}")
MESSAGE(STATUS "!!! WD = ${CMAKE_CURRENT_BINARY_DIR}")
MESSAGE(STATUS "!!! TEST dir = ${CMAKE_CURRENT_SOURCE_DIR}")


# DIRNAME is where input files are kept
# ROOT Name is the specific folder for test
FUNCTION(GFSTRINGQ_TEST DIRNAME ROOT_NAME)

	#copy input files to the testing folder
	file(COPY ${DIRNAME}${ROOT_NAME} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}${ROOT_NAME})

	TRIBITS_ADD_ADVANCED_TEST(
	gfstringq NOEXEPREFIX 
	TEST_0
		# the execution command
		EXEC gfstringq NOEXEPREFIX 
		# where the executable is
		DIRECTORY ${PROJECT_BINARY_DIR}/${PROJECT_NAME}
		# where it executes
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}${ROOT_NAME}
		# don't clean it because we copied things there that it needs
		SKIP_CLEAN_WORKING_DIRECTORY
		# the args for gstringq
		ARGS 1 4
		# where the output should go
		OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}${ROOT_NAME}scratch/paragsm000_321g
		TIMEOUT 10000
		NUM_TOTAL_CORES_USED 4
		CATEGORIES BASIC
		COMM mpi #serial
	)
ENDFUNCTION()
#call the function
#GFSTRINGQ_TEST(${CMAKE_CURRENT_SOURCE_DIR} /ethyleneRotation/qchem/de-gsm/)

if(GSM_ENABLE_QCHEM)
	GFSTRINGQ_TEST(${CMAKE_CURRENT_SOURCE_DIR} /dielsAlder/qchem/de-gsm/)
elseif(GSM_ENABLE_GAUSSIAN)
	GFSTRINGQ_TEST(${CMAKE_CURRENT_SOURCE_DIR} /dielsAlder/gaussian/de-gsm/)
elseif(GSM_ENABLE_ORCA)
	GFSTRINGQ_TEST(${CMAKE_CURRENT_SOURCE_DIR} /dielsAlder/orca/de-gsm/)
else()
	GFSTRINGQ_TEST(${CMAKE_CURRENT_SOURCE_DIR} /dielsAlder/mopac/de-gsm/)
endif()

