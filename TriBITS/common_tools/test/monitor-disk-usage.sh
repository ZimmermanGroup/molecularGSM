#!/bin/bash

#
# Send emails if the given disk partition usage is greater than the given
# percentage threadshold.
#
# Usage:
#
#   monitor-disk-usage.sh <partition> <percentage_usage> <email1> <email2> ...
#
# For example:
#
#   monitor-disk-usage.sh /home 95 my-uid@sandia.gov your-id@sandia.gov
#
# In this case, an email will be sent to your-uid@sandia.gov and
# your-id@sandia.gov if the disk containing /home is more than 95% full.
#
# This script should be run as a cron job every day (or multiple times a day).
#
# This is a stand-alone script and can be run from any directory.
#

partition=$1; shift
percentage_usage_warn=$1; shift
email_addresses="$@"

hostname=`hostname -f`

CURRENT=$(df ${partition} | grep ${partition} | awk '{ print $5}' | sed 's/%//g')

usage_str="${hostname}:${partition} ${CURRENT}% full, threshold ${percentage_usage_warn}%"

df_h_output=$(df -h ${partition})

echo "${usage_str}"
echo
echo "${df_h_output}"

if [ "${CURRENT}" -gt "${percentage_usage_warn}" ] ; then
  for email_address in ${email_addresses}; do
    echo
    echo "Sending notiviation email to ${email_address}"
    mail -s "${usage_str}" ${email_address} << EOF
WARNING: Partition ${partition} on ${hostname} is critically low!

Used: ${CURRENT}%
Warning threshold: ${percentage_usage_warn}%

${df_h_output}
EOF
  done
fi
