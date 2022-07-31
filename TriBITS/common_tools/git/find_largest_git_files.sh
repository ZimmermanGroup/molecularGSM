#!/bin/bash
#set -x
 
# Shows you the <N> largest objects in your repo's pack file.
#
# Run using:
#
#   $ cd <git-repo-dir>
#   $ ~/find_largest_git_files.sh <N>
#
# where <N> can be any positive integer (i.e.  10, 50, 100, 200, etc.).
#
# @see https://stubbisms.wordpress.com/2009/07/10/git-script-to-show-largest-pack-objects-and-trim-your-waist-line/
# @author Antony Stubbs
#
# I (Ross Bartlett) made a few modifications from the original script:
#
# 1) Removed the division by /1024 because it did not allow printing smaller
# sizes.  Now the script reports sizes in bytes, not kilobites
#
# 2) Accept number <N> in command-line argument for number of files to report
# sizes for.
 
# set the internal field spereator to line break, so that we can iterate easily over the verify-pack output
IFS=$'\n';
 
# list all objects including their size, sort by size
objects=`git verify-pack -v .git/objects/pack/pack-*.idx | grep -v chain | sort -k3nr | head -n $1`
 
echo "All sizes are in bytes. The pack column is the size of the object, compressed, inside the pack file."
 
output="size,pack,SHA,location"
for y in $objects
do
    # extract the size in bytes
    size=$((`echo $y | cut -f 5 -d ' '`))
    # extract the compressed size in bytes
    compressedSize=$((`echo $y | cut -f 6 -d ' '`))
    # extract the SHA
    sha=`echo $y | cut -f 1 -d ' '`
    # find the objects location in the repository tree
    other=`git rev-list --all --objects | grep $sha`
    #lineBreak=`echo -e "\n"`
    output="${output}\n${size},${compressedSize},${other}"
done
 
echo -e $output | column -t -s ', '
