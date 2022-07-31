#!/bin/bash
dir=$1
if [[ -d ${dir} ]] ; then
  rm -r ${dir}
fi
