#!/bin/bash
# 测试位置参数与其他特殊参数
# 使用方法 ./02parameter.sh param1 param2
# ./parameter.sh 1 2
# ./parameter.sh "1 2" 3

IFS=#   #设置分隔符是#
echo shell script name is:$0    #脚本名称
echo the count of parameters:$# #参数数量
echo first param=$1     #第一个参数
echo second param=$2    #第二个参数
echo '$*'=$*            #显示所有参数   单引号括起来表示原封不动的输出
echo '"$*"='"$*"        #显示所有参数，用#分隔
echo '$@'=$@        #未加双引号时与$*含义相同，加双引号时有区别
echo '"$@"='"$@" 
echo '$$='$$        #脚本运行的当前进程号
echo '$!='$!        #最后一个运行的后台进程号
echo '$?'=$?        #前面最后一个命令的退出状态