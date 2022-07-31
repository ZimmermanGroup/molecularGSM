#!/bin/bash -e

FILE_TO_EDIT=$1
LINE_TO_ADD=$2

echo "$LINE_TO_ADD" >> $FILE_TO_EDIT
