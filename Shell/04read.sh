#!/bin/bash
read var #从标准输入读取一行并赋值给var
read     #从标准输入读取一行并赋值给内置变量REPLY
read -a arr #读入一组词，依次赋值给数组arr

echo $var
echo $REPLY
echo ${arr[*]}

read -p "please input 3 digits:" -t 10 -a arr #-p表示提示信息 -t表示等待时间
echo ${arr[*]}
