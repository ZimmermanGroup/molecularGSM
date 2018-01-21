# TEST_DIR is passed as an argument to GFSTRINGQ_TEST function
FUNCTION(GFSTRINGQ_TEST TEST_DIR TEST_NAME)

 		#copy input files to the testing folder
		file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_DIR}/ DESTINATION
${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME})

	TRIBITS_ADD_ADVANCED_TEST( 
    ${TEST_NAME}
    TEST_0
        MESSAGE "-- Working directory is
        ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_NAME}"
        # the execution command
        EXEC gsm.${CALCULATOR} NOEXEPREFIX 
				DIRECTORY ${PROJECT_BINARY_DIR}/${PROJECT_NAME}
        # the args for gstringq, 1 is the file name and 4 is the number of processors
        ARGS 1 4
        # where it executes
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}
        # don't clean it because we copied things there that it needs
        SKIP_CLEAN_WORKING_DIRECTORY
        OUTPUT_FILE
        ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}/scratch/test_output
        NO_ECHO_OUTPUT
        PASS_ANY
 				COMM serial
        TIMEOUT 100000

    TEST_1
        MESSAGE "-- Comparing output in ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}"
        # the execution command
				CMND python
        # the args for CMND
        ARGS compare.py
        # where it executes
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}
        SKIP_CLEAN_WORKING_DIRECTORY
        # where the output should go
        OUTPUT_FILE
        ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}/scratch/compare_output
        #NO_ECHO_OUTPUT
        # don't clean it because we copied things there that it needs
        SKIP_CLEAN_WORKING_DIRECTORY
        FAIL_FAST
        NUM_TOTAL_CORES_USED 1
        CATEGORIES BASIC
        COMM serial

	)
ENDFUNCTION()

#call the function
if(GSM_ENABLE_QCHEM)
    MESSAGE(STATUS "QCHEM is enabled.")
    GFSTRINGQ_TEST(alanineDipeptideIsomerization/qchem/de-gsm alanine)
    GFSTRINGQ_TEST(ammoniaBorane/qchem/de-gsm ammonia)
    GFSTRINGQ_TEST(dielsAlder/qchem/de-gsm diels)
    GFSTRINGQ_TEST(dielsAlder/qchem/se-gsm se-diels)
    GFSTRINGQ_TEST(ethyleneRotation/qchem/de-gsm ethylene)
    GFSTRINGQ_TEST(methanolFormaldehydeHydTransfer/qchem/de-gsm methanol)

elseif(GSM_ENABLE_GAUSSIAN)
    MESSAGE(STATUS "GAUSSIAN is enabled.")
    #GFSTRINGQ_TEST(alanineDipeptideIsomerization/gaussian/de-gsm alanine)
    #GFSTRINGQ_TEST(ammoniaBorane/gaussian/de-gsm ammonia)
    GFSTRINGQ_TEST(dielsAlder/gaussian/de-gsm diels)
    GFSTRINGQ_TEST(dielsAlder/gaussian/se-gsm se-diels)
    #GFSTRINGQ_TEST(ethyleneRotation/gaussian/de-gsm ethylene)
    #GFSTRINGQ_TEST(methanolFormaldehydeHydTransfer/gaussian/de-gsm methanol)

elseif(GSM_ENABLE_ORCA)
    MESSAGE(STATUS "ORCA is enabled.")
    #GFSTRINGQ_TEST(alanineDipeptideIsomerization/orca/de-gsm alanine)
  	#GFSTRINGQ_TEST(ammoniaBorane/orca/de-gsm ammonia)
    GFSTRINGQ_TEST(dielsAlder/orca/de-gsm diels)
    GFSTRINGQ_TEST(dielsAlder/orca/se-gsm se-diels)
    #GFSTRINGQ_TEST(ethyleneRotation/orca/de-gsm ethylene)
    #GFSTRINGQ_TEST(methanolFormaldehydeHydTransfer/orca/de-gsm methanol)
elseif(GSM_ENABLE_ASE)
		MESSAGE(STATUS "ASE is enabled.")
    GFSTRINGQ_TEST(dielsAlder/ase/de-gsm diels)
elseif(GSM_ENABLE_TURBOMOLE)
		MESSAGE(STATUS "TURBOMOLE is enabled.")
		GFSTRINGQ_TEST(dielsAlder/turbomole/de-gsm-wrapper diels-w-wrapper)
		GFSTRINGQ_TEST(dielsAlder/turbomole/de-gsm-nowrapper diels)
else()
    MESSAGE(STATUS "MOPAC is enabled.")
    #GFSTRINGQ_TEST(alanineDipeptideIsomerization/mopac/de-gsm alanine)
    #GFSTRINGQ_TEST(ammoniaBorane/mopac/de-gsm ammonia)
		GFSTRINGQ_TEST(dielsAlder/mopac/de-gsm diels)
    GFSTRINGQ_TEST(ethyleneRotation/mopac/de-gsm ethylene)
    GFSTRINGQ_TEST(methanolFormaldehydeHydTransfer/mopac/de-gsm methanol)
    GFSTRINGQ_TEST(dielsAlder/mopac/se-gsm se-diels)
endif()
