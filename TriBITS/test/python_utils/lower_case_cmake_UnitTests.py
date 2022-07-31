#!/usr/bin/env python
# -*- coding: utf-8 -*-

################################################################################
# Unit testing code for lower_case_cmake.py                                    #
################################################################################


import sys
import imp
import shutil

from unittest_helpers import *

pythonDir = os.path.abspath(GeneralScriptSupport.getScriptBaseDir())
utilsDir = pythonDir+"/utils"
tribitsDir = os.path.abspath(pythonDir+"/..")

sys.path = [pythonUtilsDir] + sys.path

import lower_case_cmake as LCC


class test_makeCmndsLowerCaseInCMakeStr(unittest.TestCase):

  def test_upper_1(self):
    cmakeCodeStrIn = "\n"+\
      "SET(SOME_VAR some_value)\n"+\
      "\n"
    cmakeCodeStrOut_expected = "\n"+\
      "set(SOME_VAR some_value)\n"+\
      "\n"
    cmakeCodeStrOut = LCC.makeCmndsLowerCaseInCMakeStr(cmakeCodeStrIn)
    self.assertEqual(cmakeCodeStrOut, cmakeCodeStrOut_expected)

  def test_mixed_1(self):
    cmakeCodeStrIn = "\n"+\
      "  SET (SOME_VAR some_value)\n"+\
      "\n"+\
      "  # Some comment\n"+\
      "  Some_Longer_Func(SOME_VAR some_value)\n"+\
      "\n"+\
      "  other_functioNS\n"+\
      "    (\n"+\
      "      SOME_VAR some_value)\n"+\
      "\n"
    cmakeCodeStrOut_expected = "\n"+\
      "  set (SOME_VAR some_value)\n"+\
      "\n"+\
      "  # Some comment\n"+\
      "  some_longer_func(SOME_VAR some_value)\n"+\
      "\n"+\
      "  other_functioNS\n"+\
      "    (\n"+\
      "      SOME_VAR some_value)\n"+\
      "\n"
    cmakeCodeStrOut = LCC.makeCmndsLowerCaseInCMakeStr(cmakeCodeStrIn)
    self.assertEqual(cmakeCodeStrOut, cmakeCodeStrOut_expected)

  def test_notmacro(self):
    cmakeCodeStrIn = "\n"+\
      "# This is a macro definition!\n"+\
      "MACRO(ARG1 ARG2)\n"+\
      "# This is not a macro definition!\n"+\
      "NOTMACRO(ARG1 ARG2)\n"+\
      "\n"
    cmakeCodeStrOut_expected = "\n"+\
      "# This is a macro definition!\n"+\
      "macro(arg1 ARG2)\n"+\
      "# This is not a macro definition!\n"+\
      "notmacro(ARG1 ARG2)\n"+\
      "\n"
    cmakeCodeStrOut = LCC.makeCmndsLowerCaseInCMakeStr(cmakeCodeStrIn)
    self.assertEqual(cmakeCodeStrOut, cmakeCodeStrOut_expected)

  def test_notfunction(self):
    cmakeCodeStrIn = "\n"+\
      "# This is a function definition!\n"+\
      "FUNCTION(ARG1 ARG2)\n"+\
      "# This is not a function definition!\n"+\
      "NOTFUNCTION(ARG1 ARG2)\n"+\
      "\n"
    cmakeCodeStrOut_expected = "\n"+\
      "# This is a function definition!\n"+\
      "function(arg1 ARG2)\n"+\
      "# This is not a function definition!\n"+\
      "notfunction(ARG1 ARG2)\n"+\
      "\n"
    cmakeCodeStrOut = LCC.makeCmndsLowerCaseInCMakeStr(cmakeCodeStrIn)
    self.assertEqual(cmakeCodeStrOut, cmakeCodeStrOut_expected)

  def test_macro_def_1(self):
    cmakeCodeStrIn = "\n"+\
      "# Some macro we are defining\n"+\
      "#\n"+\
      "# Usage::\n"+\
      "#\n"+\
      "#   SOME_MACRO(<some_args>)\n"+\
      "#\n"+\
      "Macro(Some_Macro1  ARG1  ARG2)\n"+\
      "  #SOME_CALL()\n"+\
      "EndMacro()\n"+\
      "\n"+\
      "MACRO (Some_Macro2)\n"+\
      "ENDMACRO()\n"+\
      "\n"+\
      "macrO\n"+\
      "  (SOME_MACRO3)\n"+\
      "ENDMACRO()\n"+\
      "\n"+\
      "MACRO\n"+\
      "  (\n"+\
      "    SOME_MACRO4)\n"+\
      "ENDMACRO()\n"+\
      "\n"
    cmakeCodeStrOut_expected = "\n"+\
      "# Some macro we are defining\n"+\
      "#\n"+\
      "# Usage::\n"+\
      "#\n"+\
      "#   some_macro(<some_args>)\n"+\
      "#\n"+\
      "macro(some_macro1  ARG1  ARG2)\n"+\
      "  #some_call()\n"+\
      "endmacro()\n"+\
      "\n"+\
      "MACRO (Some_Macro2)\n"+\
      "endmacro()\n"+\
      "\n"+\
      "macrO\n"+\
      "  (SOME_MACRO3)\n"+\
      "endmacro()\n"+\
      "\n"+\
      "MACRO\n"+\
      "  (\n"+\
      "    SOME_MACRO4)\n"+\
      "endmacro()\n"+\
      "\n"
    cmakeCodeStrOut = LCC.makeCmndsLowerCaseInCMakeStr(cmakeCodeStrIn)
    self.assertEqual(cmakeCodeStrOut, cmakeCodeStrOut_expected)

  def test_function_def_1(self):
    cmakeCodeStrIn = "\n"+\
      "# Some function we are defining\n"+\
      "#\n"+\
      "# Usage::\n"+\
      "#\n"+\
      "#   SOME_FUNCTION(<some_args>)\n"+\
      "#\n"+\
      "Function(Some_Function1  ARG1  ARG2)\n"+\
      "  #SOME_CALL()\n"+\
      "EndFunction()\n"+\
      "\n"+\
      "FUNCTION (Some_Function2)\n"+\
      "ENDFUNCTION()\n"+\
      "\n"+\
      "function\n"+\
      "  (SOME_FUNCTION3)\n"+\
      "ENDFUNCTION()\n"+\
      "\n"+\
      "FUNCTION\n"+\
      "  (\n"+\
      "    SOME_FUNCTION4)\n"+\
      "ENDFUNCTION()\n"+\
      "\n"
    cmakeCodeStrOut_expected = "\n"+\
      "# Some function we are defining\n"+\
      "#\n"+\
      "# Usage::\n"+\
      "#\n"+\
      "#   some_function(<some_args>)\n"+\
      "#\n"+\
      "function(some_function1  ARG1  ARG2)\n"+\
      "  #some_call()\n"+\
      "endfunction()\n"+\
      "\n"+\
      "FUNCTION (Some_Function2)\n"+\
      "endfunction()\n"+\
      "\n"+\
      "function\n"+\
      "  (SOME_FUNCTION3)\n"+\
      "endfunction()\n"+\
      "\n"+\
      "FUNCTION\n"+\
      "  (\n"+\
      "    SOME_FUNCTION4)\n"+\
      "endfunction()\n"+\
      "\n"
    cmakeCodeStrOut = LCC.makeCmndsLowerCaseInCMakeStr(cmakeCodeStrIn)
    self.assertEqual(cmakeCodeStrOut, cmakeCodeStrOut_expected)

  def test_no_match_logical_operators(self):
    cmakeCodeStrIn = "\n"+\
      "FUNCTION(SOME_FUNCTION)\n"+\
      "  IF ( (VAR1) AND (VAR2) )\n"+\
      "  IF ((VAR1)AND(VAR2))\n"+\
      "  IF ( (VAR1) OR (VAR2) )\n"+\
      "  IF ( (VAR1)OR(VAR2) )\n"+\
      "  IF ( (VAR1) AND NOT (VAR2) )\n"+\
      "  IF ( (VAR1)AND NOT(VAR2) )\n"+\
      "ENDFUNCTION()\n"+\
      "\n"
    cmakeCodeStrOut_expected = "\n"+\
      "function(some_function)\n"+\
      "  if ( (VAR1) AND (VAR2) )\n"+\
      "  if ((VAR1)AND(VAR2))\n"+\
      "  if ( (VAR1) OR (VAR2) )\n"+\
      "  if ( (VAR1)OR(VAR2) )\n"+\
      "  if ( (VAR1) AND NOT (VAR2) )\n"+\
      "  if ( (VAR1)AND NOT(VAR2) )\n"+\
      "endfunction()\n"+\
      "\n"
    cmakeCodeStrOut = LCC.makeCmndsLowerCaseInCMakeStr(cmakeCodeStrIn)
    self.assertEqual(cmakeCodeStrOut, cmakeCodeStrOut_expected)

  def test_control_statements(self):
    cmakeCodeStrIn = "\n"+\
      "SET(var)\n"+\
      "SET (var)\n"+\
      "Set (var)\n"+\
      "SET  (var)\n"+\
      "IF(var)\n"+\
      "IF (var)\n"+\
      "If (var)\n"+\
      "IF  (var)\n"+\
      "ELSEIF(var)\n"+\
      "ELSEIF (var)\n"+\
      "ElseIf (var)\n"+\
      "ELSEIF  (var)\n"+\
      "FOREACH(var)\n"+\
      "FOREACH (var)\n"+\
      "Foreach (var)\n"+\
      "FOREACH  (var)\n"+\
      "WHILE(var)\n"+\
      "WHILE (var)\n"+\
      "While (var)\n"+\
      "WHILE  (var)\n"+\
      "\n"
    cmakeCodeStrOut_expected = "\n"+\
      "set(var)\n"+\
      "set (var)\n"+\
      "set (var)\n"+\
      "set  (var)\n"+\
      "if(var)\n"+\
      "if (var)\n"+\
      "if (var)\n"+\
      "if  (var)\n"+\
      "elseif(var)\n"+\
      "elseif (var)\n"+\
      "elseif (var)\n"+\
      "elseif  (var)\n"+\
      "foreach(var)\n"+\
      "foreach (var)\n"+\
      "foreach (var)\n"+\
      "foreach  (var)\n"+\
      "while(var)\n"+\
      "while (var)\n"+\
      "while (var)\n"+\
      "while  (var)\n"+\
      "\n"
    cmakeCodeStrOut = LCC.makeCmndsLowerCaseInCMakeStr(cmakeCodeStrIn)
    self.assertEqual(cmakeCodeStrOut, cmakeCodeStrOut_expected)


if __name__ == '__main__':
  unittest.main()
