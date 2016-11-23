MESSAGE(STATUS "exe = gfstringq${${PROJECT_NAME}_CMAKE_EXECUTABLE_SUFFIX}")
MESSAGE(STATUS "location = ${PROJECT_BINARY_DIR}/${PROJECT_NAME}")
MESSAGE(STATUS "test location = ${CMAKE_CURRENT_BINARY_DIR}")
MESSAGE(STATUS "!!!!Mina TEST = ${DIRNAME}${ROOT_NAME}")
MESSAGE(STATUS "!!! WD = ${CMAKE_CURRENT_BINARY_DIR}")
MESSAGE(STATUS "!!! TEST dir = ${CMAKE_CURRENT_SOURCE_DIR}")


# DIRNAME is where input files are kept
# ROOT Name is the specific folder for test
FUNCTION(GFSTRINGQ_TEST TEST_DIR)

	#copy input files to the testing folder
    #    #add_custom_command(TARGET gfstringq POST_BUILD
    #    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    #        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/${PROJECT_NAME}/gfstringq.exe 
    #        ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_DIR}/
    #        COMMENT "************Cpoying ....."
    #        )

    #MESSAGE(STATUS "Copying ${GFSTRINGQ_TEST} from
    #${PROJECT_BINARY_DIR}/${PROJECT_NAME} to
    #${CMAKE_CURRENT_SOURCE_DIR}/${TEST_DIR}")

	TRIBITS_ADD_ADVANCED_TEST( 
    gfstringq
    TEST_0
    #CMND cp
    #ARGS ${PROJECT_BINARY_DIR}/${PROJECT_NAME}/gfstringq.exe ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_DIR}/
    #MESSAGE "*** Copying gfstringq.exe"
    #TIMEOUT 20
    #TEST_1
        MESSAGE "*** Working directory is
        ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_DIR}"
        # the execution command
        EXEC gfstringq NOEXEPREFIX DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_DIR}
        # where the executable is
        #        #DIRECTORY ${PROJECT_BINARY_DIR}/${PROJECT_NAME}
        # where it executes
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_DIR}
        # don't clean it because we copied things there that it needs
        SKIP_CLEAN_WORKING_DIRECTORY
        # the args for gstringq
        ARGS 1 4
        # where the output should go
        OUTPUT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_DIR}/scratch/STDOUT
        TIMEOUT 10000
        #PASS_ANY
        FAIL_FAST
        NUM_TOTAL_CORES_USED 4
        CATEGORIES BASIC
        COMM mpi #serial
	)
ENDFUNCTION()
#call the function
#GFSTRINGQ_TEST(${CMAKE_CURRENT_SOURCE_DIR} /ethyleneRotation/qchem/de-gsm/)

if(GSM_ENABLE_QCHEM)
    MESSAGE(STATUS "!!!!!!!!!!!!!!!!!!!!!QCHEM enabled")
    file(COPY ${PROJECT_BINARY_DIR}/${PROJECT_NAME}/gfstringq.exe DESTINATION  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_DIR}/)
	GFSTRINGQ_TEST(dielsAlder/qchem/de-gsm/)
elseif(GSM_ENABLE_GAUSSIAN)
	GFSTRINGQ_TEST(${CMAKE_CURRENT_SOURCE_DIR} /dielsAlder/gaussian/de-gsm/)
elseif(GSM_ENABLE_ORCA)
	GFSTRINGQ_TEST(${CMAKE_CURRENT_SOURCE_DIR} /dielsAlder/orca/de-gsm/)
else()
	GFSTRINGQ_TEST(${CMAKE_CURRENT_SOURCE_DIR} /dielsAlder/mopac/de-gsm/)
endif()

