#!/bin/bash

echo "1----"
n=5
echo $[$n+1]        #$[] 算数表达式,返回结果
echo $(($n+1))      #同上

echo -e "\n2----"
((n+=2))            #(()) 算数赋值,无返回值.
echo $n
((n=$n+2))
echo $n

echo -e "\n3----"
expr 4 + 5          #expr 算数表达式,无返回值,但会将结果输出到标准输出.

r=`expr 4 + 5`      #用命令替换,将输出到标准输出的结果赋值给r
echo $r

r=`expr 4 \* 5`      #expr 用expr进行乘法运算时,需要转义
echo $r

echo -e "\n4----"
let "n=n+1"         #let 算数赋值,无返回值
echo $n

