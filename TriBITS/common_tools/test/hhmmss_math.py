#!/usr/bin/env python
#
# This simple set of python functions makes it easy to do simple math with
# times formatted as <hr>h<min>m<sec>s.  This makes it easier to analyze
# timing data that is spit out in that form.
#

import sys
import os

def hms2s(hms):
  #print "mmss =", mmss
  hms_len = len(hms)
  h_idx = hms.find("h")
  m_idx = hms.find("m")
  s_idx = hms.find("s")
  # hours
  hours = 0.0
  if h_idx > 0:
    h_start = 0
    hours = float(hms[0:h_idx])
    # ToDo: Handle 'm' and 's'
  # minutes
  minutes = 0.0
  if m_idx > 0:
    m_start = 0
    if h_idx > 0:
      m_start = h_idx + 1
    minutes = float(hms[m_start:m_idx])
  # seconds
  seconds = 0.0
  if s_idx > 0:
    s_start = 0
    if m_idx > 0:
      s_start = m_idx + 1
    elif h_idx > 0:
      s_start = h_idx + 1
    seconds = float(hms[s_start:s_idx])
  return hours*3600 + minutes*60 + seconds

def s2hms(seconds):
  s_digits = 5
  #print ""
  #print "seconds =", seconds
  hours = int(seconds) / 3600
  #print "hours =", hours
  seconds_h_remainder = round(seconds - hours*3600, s_digits)
  #print "seconds_h_remainder =", seconds_h_remainder
  minutes = int(seconds_h_remainder) / 60
  #print "mintues =", minutes
  seconds_reminder = round(seconds_h_remainder - minutes*60, s_digits) 
  #print "seconds_reminder = ", seconds_reminder
  h_str = ""
  if hours != 0:
    h_str = str(hours)+"h"
  m_str = ""
  if minutes != 0:
    m_str = str(minutes)+"m"
  s_str = ""
  if seconds_reminder != 0.0:
    s_str = str(seconds_reminder)+"s"
  return h_str + m_str + s_str

def sub_hms(hms1, hms2):
  num1 = hms2s(hms1)
  num2 = hms2s(hms2)
  return s2hms(num1 - num2)

def div_hms(hms_num, hms_denom):
  num_num = hms2s(hms_num)
  num_denom = hms2s(hms_denom)
  return num_num/num_denom


#
# Unit test suite
#

if __name__ == '__main__':

  import unittest
  
  class test_hms2s(unittest.TestCase):
  
    def setUp(self):
      None
  
    def test_s1(self):
      self.assertEqual(hms2s("2s"), 2.0)
  
    def test_s2(self):
      self.assertEqual(hms2s("2.53s"), 2.53)
  
    def test_s3(self):
      self.assertEqual(hms2s("0m4.5s"), 4.5)
  
    def test_m1(self):
      self.assertEqual(hms2s("1m2.4s"), 62.4)
  
    def test_m2(self):
      self.assertEqual(hms2s("3m10.531s"), 190.531)
  
    def test_h1(self):
      self.assertEqual(hms2s("2h"), 7200.0)
  
    def test_h1(self):
      self.assertEqual(hms2s("2.5h"), 9000.0)
  
    def test_h2(self):
      self.assertEqual(hms2s("1h2m3s"), 3723.0)
  
    def test_h3(self):
      self.assertEqual(hms2s("1h3s"), 3603.0)
  
  class test_s2hms(unittest.TestCase):
  
    def setUp(self):
      None
  
    def test_s1(self):
      self.assertEqual(s2hms(2.0), "2.0s")
  
    def test_s2(self):
      self.assertEqual(s2hms(3.456), "3.456s")
  
    def test_s3(self):
      self.assertEqual(s2hms(60.0), "1m")
  
    def test_m1(self):
      self.assertEqual(s2hms(75.346), "1m15.346s")
  
    def test_m2(self):
      self.assertEqual(s2hms(121.25), "2m1.25s")
  
    def test_m3(self):
      self.assertEqual(s2hms(60.0), "1m")

    def test_h1(self):
      self.assertEqual(s2hms(3600.0), "1h")

    def test_h2(self):
      self.assertEqual(s2hms(3600.001), "1h0.001s")

    def test_h3(self):
      self.assertEqual(s2hms(3660.0), "1h1m")

    def test_h4(self):
      self.assertEqual(s2hms(7140.0), "1h59m")

    def test_h5(self):
      self.assertEqual(s2hms(7141.0), "1h59m1.0s")

    def test_h5(self):
      self.assertEqual(s2hms(2*3600+3*60+7.82), "2h3m7.82s")
  
  class test_sub_hms(unittest.TestCase):
  
    def setUp(self):
      None
  
    def test_1(self):
      self.assertEqual(sub_hms("2s", "1s"), "1.0s")
  
    def test_2(self):
      self.assertEqual(sub_hms("1m5.23s", "45s"), "20.23s")
  
  class test_div_hms(unittest.TestCase):
  
    def setUp(self):
      None
  
    def test_1(self):
      self.assertEqual(div_hms("2s", "1s"), 2.0)
  
    def test_2(self):
      self.assertEqual(div_hms("1s", "2s"), 0.5)
  
    def test_3(self):
      self.assertEqual(div_hms("1m50s", "55s"), 2.0)
  
    def test_4(self):
      self.assertEqual(div_hms("55s", "1m50s"), 0.5)
    
  unittest.main()
