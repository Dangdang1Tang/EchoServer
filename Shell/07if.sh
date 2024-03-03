#!/bin/bash

# if语句
echo $#
if [ $# -eq 1 ]
then
    echo 'Usage: 3 params'
    exit 1
elif [ $# -eq 2 ]
then
    echo 'Usage: 3 params'
    exit 1
fi
echo $1

