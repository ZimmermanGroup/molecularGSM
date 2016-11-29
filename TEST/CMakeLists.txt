# TEST_DIR is passed as an argument to GFSTRINGQ_TEST function
FUNCTION(GFSTRINGQ_TEST TEST_DIR)

    # create symbolic name in testing directory to the executable in the
    # binary directory
    STRING(REGEX REPLACE "/" "_" symlink_target ${TEST_DIR} )
    ADD_CUSTOM_TARGET(${symlink_target} ALL
        COMMENT "-- Creating symbolic link from ${TARGET_NAME} to
        ${PROJECT_BINARY_DIR}/${PROJECT_NAME}/${TARGET_NAME}.
        Test directory is ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_DIR}"
        COMMAND ${CMAKE_COMMAND} -E create_symlink 
        ${PROJECT_BINARY_DIR}/${PROJECT_NAME}/${TARGET_NAME}.exe
        ${TARGET_NAME}.exe
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_DIR}
        )

	TRIBITS_ADD_ADVANCED_TEST( 
    gfstringq
    TEST_0
        MESSAGE "-- Working directory is
        ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_DIR}"
        # the execution command
        EXEC gfstringq NOEXEPREFIX DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_DIR}
        # the args for gstringq, 1 is the file name and 4 is the number of
        # processors
        ARGS 1 4
        # where it executes
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_DIR}
        # don't clean it because we copied things there that it needs
        SKIP_CLEAN_WORKING_DIRECTORY
        # where the output should go
        OUTPUT_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_DIR}/scratch/test_output
        TIMEOUT 600
        FAIL_FAST
        NUM_TOTAL_CORES_USED 4
        CATEGORIES BASIC
        COMM mpi
	)
ENDFUNCTION()

#call the function
if(GSM_ENABLE_QCHEM)
    MESSAGE(STATUS "QCHEM is enabled.")
	GFSTRINGQ_TEST(alanineDipeptideIsomerization/qchem/de-gsm)
	GFSTRINGQ_TEST(ammoniumBorane/qchem/de-gsm)
	GFSTRINGQ_TEST(dielsAlder/qchem/de-gsm)
	GFSTRINGQ_TEST(ethyleneRotation/qchem/de-gsm)
	GFSTRINGQ_TEST(methanolFormaldehydeHydTransfer/qchem/de-gsm)

elseif(GSM_ENABLE_GAUSSIAN)
    MESSAGE(STATUS "GAUSSIAN is enabled.")
	GFSTRINGQ_TEST(alanineDipeptideIsomerization/gaussian/de-gsm)
	GFSTRINGQ_TEST(ammoniumBorane/gaussian/de-gsm)
	GFSTRINGQ_TEST(dielsAlder/gaussian/de-gsm)
	GFSTRINGQ_TEST(ethyleneRotation/gaussian/de-gsm)
	GFSTRINGQ_TEST(methanolFormaldehydeHydTransfer/gaussian/de-gsm)

elseif(GSM_ENABLE_ORCA)
    MESSAGE(STATUS "ORCA is enabled.")
	GFSTRINGQ_TEST(alanineDipeptideIsomerization/orca/de-gsm)
	GFSTRINGQ_TEST(ammoniumBorane/orca/de-gsm)
	GFSTRINGQ_TEST(dielsAlder/orca/de-gsm)
	GFSTRINGQ_TEST(ethyleneRotation/orca/de-gsm)
	GFSTRINGQ_TEST(methanolFormaldehydeHydTransfer/orca/de-gsm)

else()
    MESSAGE(STATUS "MOPAC is enabled.")
    GFSTRINGQ_TEST(alanineDipeptideIsomerization/mopac/de-gsm)
    GFSTRINGQ_TEST(ammoniumBorane/mopac/de-gsm)
	GFSTRINGQ_TEST(dielsAlder/mopac/de-gsm)
    GFSTRINGQ_TEST(ethyleneRotation/mopac/de-gsm)
    GFSTRINGQ_TEST(methanolFormaldehydeHydTransfer/mopac/de-gsm)
endif()
