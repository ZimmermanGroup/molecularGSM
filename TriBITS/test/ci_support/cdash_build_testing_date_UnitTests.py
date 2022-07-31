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

#########################################################
# Unit testing code for TribitsPackageFilePathUtils.py #
######################################################### 

from FindCISupportDir import *
from GeneralScriptSupport import *
from cdash_build_testing_date import *

import sys
import unittest


def assert_timezone_offset(testObj, timezoneStr, hours):
  timeZoneOffset = getTimeZoneOffset(timezoneStr)
  testObj.assertEqual(timeZoneOffset.days, 0)
  testObj.assertEqual(timeZoneOffset.seconds, hours*3600)
  testObj.assertEqual(timeZoneOffset.microseconds, 0)


class test_getTimeZoneOffset(unittest.TestCase):

  def test_utc(self):
    assert_timezone_offset(self, "UTC", 0)

  def test_edt(self):
    assert_timezone_offset(self, "EDT", 4)

  def test_est(self):
    assert_timezone_offset(self, "EST", 5)

  def test_cdt(self):
    assert_timezone_offset(self, "CDT", 5)

  def test_cst(self):
    assert_timezone_offset(self, "CST", 6)

  def test_mdt(self):
    assert_timezone_offset(self, "MDT", 6)

  def test_mst(self):
    assert_timezone_offset(self, "MST", 7)


class test_getBuildStartTimeUtcFromStr(unittest.TestCase):

  def test_utc(self):
    buildStartTime = getBuildStartTimeUtcFromStr("2019-11-16T01:02:03 UTC")
    self.assertEqual(buildStartTime.year, 2019)
    self.assertEqual(buildStartTime.month, 11)
    self.assertEqual(buildStartTime.day, 16)
    self.assertEqual(buildStartTime.hour, 1)
    self.assertEqual(buildStartTime.minute, 2)
    self.assertEqual(buildStartTime.second, 3)
    self.assertEqual(buildStartTime.microsecond, 0)
    self.assertEqual(buildStartTime.tzinfo, None)

  def test_mdt(self):
    buildStartTime = getBuildStartTimeUtcFromStr("2019-11-16T01:02:03 MDT")
    #print str(buildStartTime)
    self.assertEqual(buildStartTime.year, 2019)
    self.assertEqual(buildStartTime.month, 11)
    self.assertEqual(buildStartTime.day, 16)
    self.assertEqual(buildStartTime.hour, 7)
    self.assertEqual(buildStartTime.minute, 2)
    self.assertEqual(buildStartTime.second, 3)
    self.assertEqual(buildStartTime.microsecond, 0)
    self.assertEqual(buildStartTime.tzinfo, None)

  # ToDo: Test invalid build start times!


class test_getProjectTestingDayStartTimeDeltaFromStr(unittest.TestCase):

  def test_valid(self):
    startTimeDelta = getProjectTestingDayStartTimeDeltaFromStr("05:12")
    self.assertEqual(startTimeDelta.days, 0)
    self.assertEqual(startTimeDelta.seconds, 5*3600+12*60)
    self.assertEqual(startTimeDelta.microseconds, 0)

  # ToDo: Test invalid testing day start time!


class test_getDayIncrTimeDeltaFromInt(unittest.TestCase):

  def test_dayIncr_0(self):
    dayIncrTimeDelta = getDayIncrTimeDeltaFromInt(0)
    self.assertEqual(dayIncrTimeDelta.days, 0)
    self.assertEqual(dayIncrTimeDelta.seconds, 0)
    self.assertEqual(dayIncrTimeDelta.microseconds, 0)

  def test_dayIncr_minus_1(self):
    dayIncrTimeDelta = getDayIncrTimeDeltaFromInt(-1)
    self.assertEqual(dayIncrTimeDelta.days, -1)
    self.assertEqual(dayIncrTimeDelta.seconds, 0)
    self.assertEqual(dayIncrTimeDelta.microseconds, 0)

  def test_dayIncr_minus_5(self):
    dayIncrTimeDelta = getDayIncrTimeDeltaFromInt(-5)
    self.assertEqual(dayIncrTimeDelta.days, -5)
    self.assertEqual(dayIncrTimeDelta.seconds, 0)
    self.assertEqual(dayIncrTimeDelta.microseconds, 0)

  def test_dayIncr_plus_2(self):
    dayIncrTimeDelta = getDayIncrTimeDeltaFromInt(2)
    self.assertEqual(dayIncrTimeDelta.days, 2)
    self.assertEqual(dayIncrTimeDelta.seconds, 0)
    self.assertEqual(dayIncrTimeDelta.microseconds, 0)


class test_getgetDateStrFromDateTime(unittest.TestCase):

  def test_date_1(self):
    buildStartTime = getBuildStartTimeUtcFromStr("2019-11-16T01:02:03 UTC")
    dateStr = getDateStrFromDateTime(buildStartTime)
    self.assertEqual(dateStr, "2019-11-16")

  def test_date_2(self):
    buildStartTime = getBuildStartTimeUtcFromStr("2018-01-03T01:02:03 UTC")
    dateStr = getDateStrFromDateTime(buildStartTime)
    self.assertEqual(dateStr, "2018-01-03")

  # ToDo: Test invalid build start times!


class test_getTestingDayDateFromBuildStartTimeStr(unittest.TestCase):

  def test_0102_utc_0000_utc(self):
    self.assertEqual(
      getTestingDayDateFromBuildStartTimeStr("2019-11-16T01:02:03 UTC",
         datetime.timedelta(hours=0)),
      "2019-11-16" )

  def test_0102_utc_0200_utc(self):
    self.assertEqual(
      getTestingDayDateFromBuildStartTimeStr("2019-11-16T01:02:03 UTC",
         datetime.timedelta(hours=2)),
      "2019-11-15" )

  def test_0102_utc_1149_utc(self):
    self.assertEqual(
      getTestingDayDateFromBuildStartTimeStr("2019-11-16T01:02:03 UTC",
         datetime.timedelta(hours=11, minutes=49)),
      "2019-11-15" )

  def test_0102_utc_1200_utc(self):
    self.assertEqual(
      getTestingDayDateFromBuildStartTimeStr("2019-11-16T01:02:03 UTC",
         datetime.timedelta(hours=12, minutes=0)),
      "2019-11-15" )

  def test_0102_utc_1201_utc(self):
    self.assertEqual(
      getTestingDayDateFromBuildStartTimeStr("2019-11-16T01:02:03 UTC",
         datetime.timedelta(hours=12, minutes=1)),
      "2019-11-16" )

  def test_1822_mdt_0000_utc(self):
    self.assertEqual(
      getTestingDayDateFromBuildStartTimeStr("2019-11-15T18:22:45 MDT",
         datetime.timedelta(hours=0)),
      "2019-11-16" )

  def test_1822_mdt_0100_utc(self):
    self.assertEqual(
      getTestingDayDateFromBuildStartTimeStr("2019-11-15T18:22:45 MDT",
         datetime.timedelta(hours=1)),
      "2019-11-15" )


class test_getRelativeCDashBuildStartTimeFromCmndLineArgs(unittest.TestCase):

  def test_previous_day_late(self):
    relBuildStartTime = getRelativeCDashBuildStartTimeFromCmndLineArgs(
      "2018-01-15T03:50:30 UTC", "04:10", 0)
    relBuildStartTime_expected = getBuildStartTimeUtcFromStr(
      "2018-01-14T23:40:30 UTC")
    self.assertEqual(relBuildStartTime, relBuildStartTime_expected)
    self.assertEqual(getDateStrFromDateTime(relBuildStartTime), "2018-01-14")

  def test_previous_day_last(self):
    relBuildStartTime = getRelativeCDashBuildStartTimeFromCmndLineArgs(
      "2018-01-15T04:09:59 UTC", "04:10", 0)
    relBuildStartTime_expected = getBuildStartTimeUtcFromStr(
      "2018-01-14T23:59:59 UTC")
    self.assertEqual(relBuildStartTime, relBuildStartTime_expected)
    self.assertEqual(getDateStrFromDateTime(relBuildStartTime), "2018-01-14")

  def test_same_day_first(self):
    relBuildStartTime = getRelativeCDashBuildStartTimeFromCmndLineArgs(
      "2018-01-15T04:10:00 UTC", "04:10", 0)
    relBuildStartTime_expected = getBuildStartTimeUtcFromStr(
      "2018-01-15T00:00:00 UTC")
    self.assertEqual(relBuildStartTime, relBuildStartTime_expected)
    self.assertEqual(getDateStrFromDateTime(relBuildStartTime), "2018-01-15")

  def test_same_day_early(self):
    relBuildStartTime = getRelativeCDashBuildStartTimeFromCmndLineArgs(
      "2018-01-15T04:50:29 UTC", "04:10", 0)
    relBuildStartTime_expected = getBuildStartTimeUtcFromStr(
      "2018-01-15T00:40:29 UTC")
    self.assertEqual(relBuildStartTime, relBuildStartTime_expected)
    self.assertEqual(getDateStrFromDateTime(relBuildStartTime), "2018-01-15")

  def test_same_day_mid(self):
    relBuildStartTime = getRelativeCDashBuildStartTimeFromCmndLineArgs(
      "2018-01-15T12:44:20 UTC", "04:10", 0)
    relBuildStartTime_expected = getBuildStartTimeUtcFromStr(
      "2018-01-15T08:34:20 UTC")
    self.assertEqual(relBuildStartTime, relBuildStartTime_expected)
    self.assertEqual(getDateStrFromDateTime(relBuildStartTime), "2018-01-15")


class test_CDashProjectTestingDay(unittest.TestCase):

  def test_construct_mignight(self):
    cdashProjectTestingDayObj = CDashProjectTestingDay("2019-05-22", "00:00")
    self.assertEqual(
      getBuildStartTimeUtcStrFromUtcDT(
        cdashProjectTestingDayObj.getTestingDayStartUtcDT() ),
      "2019-05-22T00:00:00 UTC" )
    self.assertEqual(
      getBuildStartTimeUtcStrFromUtcDT(
        cdashProjectTestingDayObj.getTestingDayStartUtcDT(), True ),
      "2019-05-22T00:00:00UTC" )

  def test_construct_0302_am(self):
    cdashProjectTestingDayObj = CDashProjectTestingDay("2019-05-22", "03:20")
    self.assertEqual(
      getBuildStartTimeUtcStrFromUtcDT(
        cdashProjectTestingDayObj.getCurrentTestingDayDateDT() ),
      "2019-05-22T00:00:00 UTC" )
    self.assertEqual(
      getBuildStartTimeUtcStrFromUtcDT(
        cdashProjectTestingDayObj.getTestingDayStartUtcDT() ),
      "2019-05-22T03:20:00 UTC" )

  def test_construct_just_before_noon(self):
    cdashProjectTestingDayObj = CDashProjectTestingDay("2019-05-22", "11:59")
    self.assertEqual(
      getBuildStartTimeUtcStrFromUtcDT(
        cdashProjectTestingDayObj.getTestingDayStartUtcDT() ),
      "2019-05-22T11:59:00 UTC" )

  def test_construct_noon(self):
    cdashProjectTestingDayObj = CDashProjectTestingDay("2019-05-22", "12:00")
    self.assertEqual(
      getBuildStartTimeUtcStrFromUtcDT(
        cdashProjectTestingDayObj.getTestingDayStartUtcDT() ),
      "2019-05-21T12:00:00 UTC" )

  def test_construct_just_after_noon(self):
    cdashProjectTestingDayObj = CDashProjectTestingDay("2019-05-22", "12:01")
    self.assertEqual(
      getBuildStartTimeUtcStrFromUtcDT(
        cdashProjectTestingDayObj.getTestingDayStartUtcDT() ),
      "2019-05-21T12:01:00 UTC" )

  def test_construct_6_pm(self):
    cdashProjectTestingDayObj = CDashProjectTestingDay("2019-05-22", "18:00")
    self.assertEqual(
      getBuildStartTimeUtcStrFromUtcDT(
        cdashProjectTestingDayObj.getTestingDayStartUtcDT() ),
      "2019-05-21T18:00:00 UTC" )

  def test_0102_utc_0000_utc(self):
    cptdo = CDashProjectTestingDay("2019-05-22", "00:00")
    self.assertEqual(
      cptdo.getTestingDayDateFromBuildStartTimeStr("2019-11-16T01:02:03 UTC"),
      "2019-11-16" )

  def test_0102_utc_0000_utc(self):
    cptdo = CDashProjectTestingDay("2019-05-22", "02:00")
    self.assertEqual(
      cptdo.getTestingDayDateFromBuildStartTimeStr("2019-11-16T01:02:03 UTC"),
      "2019-11-15" )

  def test_1822_mdt_0000_utc(self):
    cptdo = CDashProjectTestingDay("2019-05-22", "00:00")
    self.assertEqual(
      cptdo.getTestingDayDateFromBuildStartTimeStr("2019-11-15T18:22:45 MDT"),
      "2019-11-16" )

  def test_1722_mdt_0000_utc(self):
    cptdo = CDashProjectTestingDay("2019-05-22", "00:00")
    self.assertEqual(
      cptdo.getTestingDayDateFromBuildStartTimeStr("2019-11-15T17:22:45 MDT"),
      "2019-11-15" )

  def test_0102_utc_1200_utc(self):
    cptdo = CDashProjectTestingDay("2019-05-22", "12:00")
    self.assertEqual(
      cptdo.getTestingDayDateFromBuildStartTimeStr("2019-11-16T01:02:03 UTC"),
      "2019-11-15" )

  def test_0102_utc_1201_utc(self):
    cptdo = CDashProjectTestingDay("2019-05-22", "12:01")
    self.assertEqual(
      cptdo.getTestingDayDateFromBuildStartTimeStr("2019-11-16T01:02:03 UTC"),
      "2019-11-16" )


# Utility function to make it easy to test the script itself
def run_cdash_build_testing_date_py_test(testObj, cmndArgs, testingDay_expected):
  cmnd = ciSupportDir+"/cdash_build_testing_date.py "+" ".join(cmndArgs)
  testingDay = getCmndOutput(cmnd, True)
  testObj.assertEqual(testingDay, testingDay_expected)


#
# Test the script cdash_build_testing_date.py itself, but we only need to do
# some very basic checking to make sure that it accepts the right command-line
# args and returns what we expect for some simple use cases.  The more
# detailed testing is taken care of in the above unit tests.
#
class test_cdash_build_testing_date_py(unittest.TestCase):

  def test_previous_day(self):
    cmndArgs = [
       "--cdash-project-start-time='05:00'",
       "--cdash-build-start-time='2019-01-15T03:00:00 UTC'",
       ]
    testingDay_expected = "2019-01-14"
    run_cdash_build_testing_date_py_test(self, cmndArgs, testingDay_expected)

  def test_today(self):
    cmndArgs = [
       "--cdash-project-start-time='05:00'",
       "--cdash-build-start-time='2019-01-15T06:00:00 UTC'",
       ]
    testingDay_expected = "2019-01-15"
    run_cdash_build_testing_date_py_test(self, cmndArgs, testingDay_expected)

  def test_today_dayIncr_minus_3(self):
    cmndArgs = [
       "--cdash-project-start-time='05:00'",
       "--cdash-build-start-time='2019-01-15T06:00:00 UTC'",
       "--day-incr=-3",
       ]
    testingDay_expected = "2019-01-12"
    run_cdash_build_testing_date_py_test(self, cmndArgs, testingDay_expected)
    # NOTE: Above, we just test that --day-incr is accepted and has the right
    # behavior.

  def test_right_now_previous_day(self):
    nowUtc = datetime.datetime.utcnow()
    if nowUtc.minute+1 >= 60:
      return  # Skip the test if ran at 59 minutes after hour!
    testingDayStartTime = str(nowUtc.hour)+":"+str(nowUtc.minute+1) 
    cmndArgs = [
       "--cdash-project-start-time='"+testingDayStartTime+"'",
       ]
    testingDay_expected = (nowUtc-datetime.timedelta(hours=24)).strftime("%Y-%m-%d")
    run_cdash_build_testing_date_py_test(self, cmndArgs, testingDay_expected)
    # NOTE: Above, we make sure that the correct UTC time is used when the
    # argument --cdash-build-start-time is not passed in (which is the most
    # important use case).

  def test_right_now_next_day(self):
    nowUtc = datetime.datetime.utcnow()
    if nowUtc.minute-1 < 0:
      return  # Skip the test if ran at 0 minutes after the hour
    testingDayStartTime = str(nowUtc.hour)+":"+str(nowUtc.minute-1) 
    cmndArgs = [
       "--cdash-project-start-time='"+testingDayStartTime+"'",
       ]
    testingDay_expected = nowUtc.strftime("%Y-%m-%d")
    run_cdash_build_testing_date_py_test(self, cmndArgs, testingDay_expected)
    # NOTE: Above, we make sure that the correct UTC time is used when the
    # argument --cdash-build-start-time is not passed in (which is the most
    # important use case).

  

if __name__ == '__main__':

  from GetWithCmake import *

  unittest.main()
