#!/bin/bash


if [ $# -ne 1 ]
then
    echo 'usage: $0 <n>'
    exit 1
fi

if [ $1 -lt "5" -o $1 -gt "15" ]
then
    echo 'usage: $0 <n>'
    echo ' where 5<=n<=15'
    exit 1
fi

for ((i=0;i<$1;i++))
do
    for ((j=0;j<$[$1-$i-1];j++))
    do
        echo -n " "
    done

    for ((j=0;j<$[2*$i+1];j++))
    do
        echo -n "*"
    done

    echo -ne '\n'
done


echo 用while实现
lines=$1
curline=0
while [ $curline -lt $lines ]
do
    nSpace=$[$lines-$curline-1]
    while [ $nSpace -gt 0 ]
    do
        echo -n " "
        nSpace=$[$nSpace-1]
    done
    nStars=$[2*$curline+1]
    while [ $nStars -gt 0 ]
    do
        echo -n "*"
        nStars=$[$nStars-1]
    done
    echo
    curline=$[$curline+1]
done
