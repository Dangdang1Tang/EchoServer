#!/bin/bash
arr=(math english chinese)
echo ${arr[0]}
echo ${arr[1]}
echo ${arr[2]}
echo ${arr[3]}
echo ${arr[*]}
echo ${#arr[*]}
arr[6]=bb
echo ${arr[6]}  #shll中数组下标可以不连续，区别于其他语言
echo ${#arr[*]}
