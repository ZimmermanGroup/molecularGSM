#
# Run to read and print env vars.
#
# Usage:
#
#   cmake \
#     -DENV_VAR_0=<envvarname0> \
#     -DENV_VAR_1=<envvarname1> \
#     ...
#
# and it read and print these env vars up to 10
#

cmake_minimum_required(VERSION 3.17.0 FATAL_ERROR)

set(maxNumEnvVars 10)

foreach(idx RANGE ${maxNumEnvVars})
  set(envVarCacheVarName_idx "ENV_VAR_${idx}")
  set(envVarName_idx "${${envVarCacheVarName_idx}}")
  if (NOT "${envVarName_idx}" STREQUAL "")
    set(envVarValue_idx "$ENV{${envVarName_idx}}")
    message("${envVarName_idx} = '${envVarValue_idx}'")
  endif()
endforeach()
