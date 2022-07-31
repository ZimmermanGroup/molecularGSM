import os
import sys

devtoolsInstallDir = os.path.join(
  os.path.dirname(os.path.abspath(__file__)),
  "../..", "tribits/devtools_install" )
sys.path = [devtoolsInstallDir] + sys.path
