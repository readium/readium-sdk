#!/bin/sh

root=$(pwd)

FILE_PATH="doc"
if [ "$#" -eq 0 ]
then
    echo "ERROR: script argument is missing! (FILE_PATH) Exit."
    exit 0
else
    echo "INFO: Script arguments: ${#}"
    echo "INFO: Script argument #1 (FILE_PATH): ${1}"
    FILE_PATH=${1}
    
    if test -f "${FILE_PATH}"
    then
        echo "INFO: FILE_PATH exists."
    else
        echo "ERROR: FILE_PATH does not exist! Exit."
        exit 0
    fi
fi

javac "${root}/contentFilterChainEncode.java" -d "${root}/" -sourcepath "${root}" 2> java.out.txt
#-verbose

exitValue=$?

cat java.out.txt
javaout=`cat java.out.txt`
rm java.out.txt

if [ $exitValue != 0 ] 
then
    echo "JAVA COMPILE FAILED?"
    exit $exitValue
fi

java -classpath "${root}/" contentFilterChainEncode "${FILE_PATH}"
