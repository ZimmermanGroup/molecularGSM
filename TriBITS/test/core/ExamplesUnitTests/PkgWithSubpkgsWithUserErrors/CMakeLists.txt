###############################################################
# test a passing call order, will be run by default turn this 
# off when you turn on one of the other sections
###############################################################
if (NOT ${PACKAGE_NAME}_TURN_OFF_PASSING_CALL_ORDER)
  
  print_var(${PACKAGE_NAME}_SUBPACKAGES)

  #
  # A) Forward declare the package so that certain options are also defined for
  # subpackages
  #
  tribits_package_decl(PkgWithSubpkgsWithUserErrors)

  #
  # B) Define the common options for the package first so they can be used by
  # subpackages as well.
  #
  tribits_add_debug_option()

  #
  # C) Process the subpackages
  #
  tribits_process_subpackages()

  #
  # D) Define the package now and perform standard post processing
  #
  tribits_package_def()
  tribits_package_postprocess()
endif()


###############################################################
# No PACKAGE_DECL before PROCESS_SUBPACKAGES
###############################################################
if(${PACKAGE_NAME}_no_PACKAGE_DECL_before_PROCESS_SUBPACKAGES)
  message( "Unit test ${PACKAGE_NAME}_no_PACKAGE_DECL_before_PROCESS_SUBPACKAGES")
  tribits_process_subpackages()
endif()


###############################################################
# PACKAGE_POSTPROCESS before PROCESS_SUBPACKAGES
###############################################################
if(${PACKAGE_NAME}_POSTPROCESS_before_SUBPACKAGES)
  message( "Unit test ${PACKAGE_NAME}_POSTPROCESS_before_SUBPACKAGES")
  tribits_package_decl(PkgWithSubpkgsWithUserErrors)
  tribits_package_def()
  tribits_package_postprocess()
  tribits_process_subpackages()
endif()

###############################################################
# PACKAGE_DEF before PROCESS_SUBPACKAGES
###############################################################
if(${PACKAGE_NAME}_PACKAGE_DEF_before_SUBPACKAGES)
  message( "Unit test ${PACKAGE_NAME}_PACKAGE_DEF_before_SUBPACKAGES")
  tribits_package_decl(PkgWithSubpkgsWithUserErrors)
  tribits_package_def()
  tribits_process_subpackages()
  tribits_package_postprocess()
endif()

###############################################################
# multiple calls to PACKAGE_DECL
###############################################################
if(${PACKAGE_NAME}_multiple_calls_to_PACKAGE_DECL)
  message( "Unit test ${PACKAGE_NAME}_multiple_calls_to_PACKAGE_DECL")
  tribits_package_decl(PkgWithSubpkgsWithUserErrors)
  tribits_package_decl(PkgWithSubpkgsWithUserErrors)
  tribits_add_debug_option()
  tribits_process_subpackages()
  tribits_package_def()
  tribits_package_postprocess()
endif()

###############################################################
# multiple calls to PACKAGE_DEF
###############################################################
if(${PACKAGE_NAME}_multiple_calls_to_PACKAGE_DEF)
  message( "Unit test ${PACKAGE_NAME}_multiple_calls_to_PACKAGE_DEF")
  tribits_package_decl(PkgWithSubpkgsWithUserErrors)
  tribits_add_debug_option()
  tribits_process_subpackages()
  tribits_package_def()
  tribits_package_def()
  tribits_package_postprocess()
endif()


###############################################################
# call package() from package with subpackages
###############################################################
if(${PACKAGE_NAME}_call_PACKAGE_from_package_with_subpackages)
  message( "Unit test ${PACKAGE_NAME}_call_PACKAGE_from_package_with_subpackages")
  tribits_package(PkgWithSubpkgsWithUserErrors)
  tribits_process_subpackages()
  tribits_package_postprocess()
endif()


###############################################################
#  Call PACKAGE_DECL without anything else
###############################################################
if(${PACKAGE_NAME}_call_PACKAGE_DECL_only)
  message( "Unit test ${PACKAGE_NAME}_call_PACKAGE_DECL_only")
  tribits_package_decl(PkgWithSubpkgsWithUserErrors)
  tribits_package_postprocess()
endif()


###############################################################
# call subpackage() from a package
###############################################################
if(${PACKAGE_NAME}_call_SUBPACKAGE_from_package_with_subpackages)
  message( "Unit test ${PACKAGE_NAME}_call_SUBPACKAGE_from_package_with_subpackages")
  tribits_subpackage(PkgWithSubpkgsWithUserErrors)
  tribits_package_postprocess()
endif()


# UNTESTED
###############################################################
# call SUBPACKAGE_POSTPROCESS from a package
###############################################################
if(${PACKAGE_NAME}_call_SUBPACKAGE_POSTPROCESS)
  message( "Unit test ${PACKAGE_NAME}_call_SUBPACKAGE_POSTPROCESS")
  tribits_package_decl(PkgWithSubpkgsWithUserErrors)
  tribits_process_subpackages()
  tribits_package_def()
  tribits_subpackage_postprocess()
endif()


###############################################################
# Calling PACKAGE after PROCESS_SUBPACKAGES
###############################################################
if(${PACKAGE_NAME}_call_PACKAGE_after_PROCESS_SUBPACKAGES)
  message( "Unit test ${PACKAGE_NAME}_call_PACKAGE_after_PROCESS_SUBPACKAGES")
  tribits_package_decl(PkgWithSubpkgsWithUserErrors)
  tribits_process_subpackages()
  tribits_package(PkgWithSubpkgsWithUserErrors)
  tribits_subpackage_postprocess()
endif()


###############################################################
# Not calling PROCESS_SUBPACKAGES when package has subpackages
###############################################################
if(${PACKAGE_NAME}_call_everthing_except_PROCESS_SUBPACKAGES)
  message( "Unit test ${PACKAGE_NAME}_call_everthing_except_PROCESS_SUBPACKAGES")
  tribits_package_decl(PkgWithSubpkgsWithUserErrors)
  tribits_package_def()
  tribits_package_postprocess()
endif()
