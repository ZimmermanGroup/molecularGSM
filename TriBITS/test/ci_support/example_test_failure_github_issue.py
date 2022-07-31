#!/usr/bin/env python

import os
import sys


#
# Implementation code
#

# Find the implementation
tribitsDir = os.environ.get("TRIBITS_DIR", "")
if tribitsDir:
  sys.path = [os.path.join(tribitsDir, "ci_support")] + sys.path
else:
  raise Exception("ERROR, TRIBITS_DIR must be set in the env to 'tribits' base dir!")

import CreateIssueTrackerFromCDashQuery as CITFCQ


usageHelp = \
r"""Stock error message.
"""


# The main function
def main():
  issueTrackerCreator = \
    CITFCQ.CreateIssueTrackerFromCDashQueryDriver(
      ExampleIssueTrackerFormatter(),
      cdashProjectStartTimeUtc="6:00", # Midnight MST
      usageHelp=usageHelp,
      issueTrackerUrlTemplate="https://github.com/<group>/<repo>/issues/<newissueid>",
      issueTrackerTemplate="#<newissueid>" )
  issueTrackerCreator.runDriver()


# Nonmember function to actually create the body of the new GitHub Markdown text
#
# NOTE: This was made a nonmember function to put this to the bottom and not
# obscure the implementing class 'ExampleIssueTrackerFormatter'
#
def getGithubIssueBodyMarkdown(
    itd,  # issueTrackerData (type IssueTrackerData)
  ):

  issueTrackerText = \
r"""
SUMMARY: """+itd.summaryLine+" "+itd.testingDayStartNonpassingDate+r"""

## Description

As shown in [this query]("""+itd.nonpassingTestsUrl+r""") (click "Show Matching Output" in upper right) the tests:

""" + CITFCQ.getMarkdownListStr(itd.testnameList, '`') + \
r"""
in the builds:

""" + CITFCQ.getMarkdownListStr(itd.buildnameList, '`') + \
r"""
started failing on testing day """+itd.testingDayStartNonpassingDate+r""".


## Current Status on CDash

Run the [above query]("""+itd.nonpassingTestsUrl+r""") adjusting the "Begin" and "End" dates to match today any other date range or just click "CURRENT" in the top bar to see results for the current testing day.
"""

  # NOTE: ABOVE: It is important to keep entire paragraphs on one line.
  # Otherwise, GitHub will show the line-breaks and it looks terrible.

  return issueTrackerText

# END FUNCTION: getGithubIsseBodyMarkdown()


################################################################################
#
# EVERYTHING BELOW HERE SHOULD NOT NEED TO BE MODIFIED.  IT IS JUST
# BOILERPLATE CODE
##
################################################################################


# Class implementation of callback to fill in the new ATDM Trilinos GitHub
# issue.
#
class ExampleIssueTrackerFormatter:
  def createFormattedIssueTracker(self, issueTrackerData):
    return getGithubIssueBodyMarkdown(issueTrackerData)


#
# Execute main if this is being run as a script
#

if __name__ == '__main__':
  sys.exit(main())

