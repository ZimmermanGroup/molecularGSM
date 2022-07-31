#!/bin/bash
_SCRIPT_DIR=`echo $0 | sed "s/\(.*\)\/.*[.]sh/\1/g"`
#echo $_SCRIPT_DIR
$_SCRIPT_DIR/cloc.pl --read-lang-def=${_SCRIPT_DIR}/cloc.script-languages.in "$@"
