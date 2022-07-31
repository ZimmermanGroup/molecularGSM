# @HEADER
# ************************************************************************
#
#            TriBITS: Tribal Build, Integrate, and Test System
#                    Copyright 2013 Sandia Corporation
#
# Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
# the U.S. Government retains certain rights in this software.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
# 1. Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution.
#
# 3. Neither the name of the Corporation nor the names of the
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# ************************************************************************
# @HEADER

cmake_minimum_required(VERSION 3.17.0 FATAL_ERROR)

message("PROJECT_NAME = ${PROJECT_NAME}")
message("${PROJECT_NAME}_TRIBITS_DIR = ${${PROJECT_NAME}_TRIBITS_DIR}")

set( CMAKE_MODULE_PATH
  "${${PROJECT_NAME}_TRIBITS_DIR}/core/utils"
  "${${PROJECT_NAME}_TRIBITS_DIR}/core/package_arch"
  "${${PROJECT_NAME}_TRIBITS_DIR}/ctest_driver"
  )

include(GlobalSet)
include(UnitTestHelpers)

include(TribitsReadTagFile)
include(TribitsGetCDashUrlsInsideCTestS)


function(unittest_tribits_get_cdash_revision_builds_url)

  message("\n***")
  message("*** Testing tribits_get_cdash_revision_builds_url()")
  message("***\n")

  tribits_get_cdash_revision_builds_url(
     CDASH_SITE_URL "somesite.com/my-cdash"
     PROJECT_NAME  goodProject
     GIT_REPO_SHA1 "abc123"
     CDASH_REVISION_BUILDS_URL_OUT  cdashRevesionBuildsUrlOut
     )
  unittest_compare_const(cdashRevesionBuildsUrlOut
    "somesite.com/my-cdash/index.php?project=goodProject&filtercount=1&showfilters=1&field1=revision&compare1=61&value1=abc123")

endfunction()


function(unittest_tribits_get_cdash_revision_nonpassing_tests_url)

  message("\n***")
  message("*** Testing tribits_get_cdash_revision_nonpassing_tests_url()")
  message("***\n")

  tribits_get_cdash_revision_nonpassing_tests_url(
     CDASH_SITE_URL "somesite.com/my-cdash"
     PROJECT_NAME  goodProject
     GIT_REPO_SHA1 "abc123"
     CDASH_REVISION_NONPASSING_TESTS_URL_OUT  cdashRevisionNonpassingTestsUrlOut
     )
  unittest_compare_const(cdashRevisionNonpassingTestsUrlOut
    "somesite.com/my-cdash/queryTests.php?project=goodProject&filtercount=2&showfilters=1&filtercombine=and&field1=revision&compare1=61&value1=abc123&field2=status&compare2=62&value2=passed")

endfunction()


function(unittest_tribits_read_ctest_tag_file)

  message("\n***")
  message("*** Testing tribits_read_ctest_tag_file()")
  message("***\n")

  set(TAG_FILE_IN "${CMAKE_CURRENT_LIST_DIR}/data/dummy_build_dir/Testing/TAG")

  tribits_read_ctest_tag_file(${TAG_FILE_IN} buildStartTime cdashGroup cdashModel)

  unittest_compare_const(buildStartTime
    "20101015-1112")
  unittest_compare_const(cdashGroup
    "My CDash Group")  # NOTE: Spaces are important to test here!
  unittest_compare_const(cdashModel
    "The Model")  # NOTE: Spaces are important to test here!

endfunction()


function(unittest_tribits_get_cdash_site_from_drop_site_and_location)

  message("\n***")
  message("*** Testing tribits_get_cdash_site_from_drop_site_and_location()")
  message("***\n")

  tribits_get_cdash_site_from_drop_site_and_location(
    CTEST_DROP_SITE  "some.site.com"
    CTEST_DROP_LOCATION  "/cdash/submit.php?project=SomeProject"
    CDASH_SITE_URL_OUT  cdashSiteUrl
    )

  unittest_compare_const(cdashSiteUrl "https://some.site.com/cdash")

endfunction()


function(unittest_tribits_get_cdash_index_php_from_drop_site_and_location)

  message("\n***")
  message("*** Testing tribits_get_cdash_index_php_from_drop_site_and_location()")
  message("***\n")

  tribits_get_cdash_index_php_from_drop_site_and_location(
    CTEST_DROP_SITE "some.site.com"
    CTEST_DROP_LOCATION "/cdash/submit.php?project=SomeProject"
    INDEX_PHP_URL_OUT indexPhpUrl
    )

  unittest_compare_const(indexPhpUrl "https://some.site.com/cdash/index.php")

endfunction()


function(unittest_tribits_get_cdash_build_url_from_parts)

  message("\n***")
  message("*** Testing tribits_get_cdash_build_url_from_parts()")
  message("***\n")

  tribits_get_cdash_build_url_from_parts(
    INDEX_PHP_URL "mycdash/index.php"
    PROJECT_NAME "my project"
    SITE_NAME "my site"
    BUILD_NAME "my buildname g++-2.5"
    BUILD_STAMP "20210729-0024-My Group"
    CDASH_BUILD_URL_OUT cdashBuildUrlOut
    )

  unittest_compare_const(cdashBuildUrlOut
     "mycdash/index.php?project=my%20project&filtercount=3&showfilters=1&filtercombine=and&field1=site&compare1=61&value1=my%20site&field2=buildname&compare2=61&value2=my%20buildname%20g%2B%2B-2.5&field3=buildstamp&compare3=61&value3=20210729-0024-My%20Group") 

endfunction()


function(unittest_tribits_get_cdash_build_url_from_tag_file)

  message("\n***")
  message("*** Testing tribits_get_cdash_build_url_from_tag_file()")
  message("***\n")

  set(TAG_FILE "${CMAKE_CURRENT_LIST_DIR}/data/dummy_build_dir/Testing/TAG")

  tribits_get_cdash_build_url_from_tag_file(
    INDEX_PHP_URL "mycdash/index.php"
    PROJECT_NAME "my project"
    SITE_NAME "my site"
    BUILD_NAME "my buildname g++-2.5"
    TAG_FILE "${TAG_FILE}"
    CDASH_BUILD_URL_OUT cdashBuildUrl
    )

  unittest_compare_const(cdashBuildUrl
     "mycdash/index.php?project=my%20project&filtercount=3&showfilters=1&filtercombine=and&field1=site&compare1=61&value1=my%20site&field2=buildname&compare2=61&value2=my%20buildname%20g%2B%2B-2.5&field3=buildstamp&compare3=61&value3=20101015-1112-My%20CDash%20Group") 

endfunction()


#
# Execute the unit tests
#

unittest_initialize_vars()

# Run the unit test functions
unittest_tribits_get_cdash_revision_builds_url()
unittest_tribits_get_cdash_revision_nonpassing_tests_url()
unittest_tribits_read_ctest_tag_file()
unittest_tribits_get_cdash_site_from_drop_site_and_location()
unittest_tribits_get_cdash_index_php_from_drop_site_and_location()
unittest_tribits_get_cdash_build_url_from_parts()
unittest_tribits_get_cdash_build_url_from_tag_file()

message("\n***")
message("*** Determine final result of all unit tests")
message("***\n")

# Pass in the number of expected tests that must pass!
unittest_final_result(9)
