
################################################################################
#
# Unit testing helpers for CDashQueryAnalyzeReport code
#
################################################################################

import re
import shutil


# Copy a list of files from one directory to another
def copyFilesListSrcToDestDir(origDir, filenameList, destDir):
  for filename in filenameList:
    shutil.copyfile(origDir+"/"+filename, destDir+"/"+filename)


# Find the index of a test in a list of test dicts
def getIdxOfTestInTestLOD(testsLOD, site, buildName, testname):
  testIdx = 0
  for testsDict in testsLOD:
    if testsDict['site'] == site \
      and testsDict['buildName'] == buildName \
      and testsDict['testname'] == testname \
      :
      #print(testsDict)
      break
    testIdx = testIdx+1
  return testIdx


# Search for a list of strings in a single long string
def assertFindListOfStringsInString(
  testObj,
  stringListToFind,
  stringToSearch,
  stringToSearchVarName,
  debugPrint=False,
  ):
  for stringToFind in stringListToFind:
    foundString = True
    if stringToSearch.find(stringToFind) == -1:
      foundString = False
      if debugPrint:
        print(stringToSearchVarName+" = "+str(stringToSearch))
    testObj.assertTrue(foundString,
      "Error, could not find string '"+stringToFind+"' in '"+stringToSearchVarName+"'!")


# Search for a string in a list of strings
def assertFindStringInListOfStrings(
  testObj,
  stringToFind,
  stringsList,
  stringsListName,
  ):
  foundStringToFind = False
  for stdoutLine in stringsList:
    if stdoutLine.find(stringToFind) != -1:
      foundStringToFind = True
      break
  testObj.assertTrue(foundStringToFind,
    "Error, could not find string '"+stringToFind+"' in "+stringsListName+"!")


# Search for a list of regexs in order in a list of strings
def assertListOfRegexsFoundInListOfStrs(
  testObj,
  regexList,
  stringsList,
  stringsListName,
  debugPrint=False,
  ):
  # Set up for first regex
  current_regex_idx = 0
  currentRe = re.compile(regexList[current_regex_idx])
  # Loop over the lines in the input strings list and look for the regexes in
  # order!
  strLine_idx = -1
  for strLine in stringsList:
    strLine_idx += 1
    if current_regex_idx == len(regexList):
      # Found all the regexes so we are done!
      break
    if debugPrint:
      print("\nstrLine_idx = '"+str(strLine_idx)+"'")
      print("strLine = '"+strLine+"'")
      print("regexList["+str(current_regex_idx)+"] = '"+regexList[current_regex_idx]+"'")
    if currentRe.match(strLine):
      # Found the current regex being looked for!
      if debugPrint:
        print("Found match!")
      current_regex_idx += 1
      if current_regex_idx < len(regexList):
        if debugPrint:
          print("regexList["+str(current_regex_idx)+"] = '"+regexList[current_regex_idx]+"'")
        currentRe = re.compile(regexList[current_regex_idx])
      continue
  # Look to see if you have found all of the regexes being searched for.  If
  # the current one has not been found, then report that you could not find
  # it!
  if current_regex_idx < len(regexList):
    testObj.assertTrue(False,
      "Error, could not find the regex '"+regexList[current_regex_idx]+"'"+\
      " in "+stringsListName+"!")


def assertFileContentsAsStringArray(testObj, filename, expecteStrList):
  with open(filename, 'r') as fileHandle:
    fileStrList = fileHandle.read().split("\n")
  testObj.assertEqual(fileStrList, expecteStrList)
