#!/bin/bash


for i in 1 2 3 4 5
do
    echo value is $i
done

echo 省略in===
#  如果省略了in,就会在$@中循环
for i
do
    echo value is $i
done

