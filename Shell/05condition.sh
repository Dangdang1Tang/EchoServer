#!/bin/bash

# 条件测试
# 条件测试可以根据某个特定条件是否满足,来选择执行相应的任务
# Bash中允许测试两种类型的条件:
    # 命令成功或失败,表达式为真或假
# 任何一种测试中,都要有退出状态(返回值),退出状态为0表示命令成功或表达式为真,非0表示命令失败或表达式为假.
# 状态变量$?中保存命令退出状态的值.

# 表达式测试包括字符串测试,整数测试和文件测试.
# 内置测试命令test

# 通常用test命令来测试表达式的值

x=5;y=10
test $x -gt $y  #test 测试表达式的值
echo $?

[ $x -gt $y ]   #[] test命令可以用方括号替代,方括号两边要有空格
echo $?

name=Tom
[[ $name = [Tt]?? ]]    #[[]] 可以用双中括号来测试表达式的值,此时可以使用通配符进行模式匹配
echo $?


#字符串测试
echo 字符串测试
str1=str1
str2=str2
[ -z $str1 ] #-z 如果str长度为0,返回真
echo $?
[ -n $str1 ] #-n 如果str长度不为0,返回真
echo $?
[ $str1 = $str2 ]   #两字符串相等
echo $?
[ $str1 != $str2 ]  #两字符串不等
echo $?


#整数测试
echo 整数测试 []
int1=1
int2=2
[ $int1 -eq $int2 ]    #int1等于int2
echo $?
[ $int1 -ne $int2 ]    #int1不等于int2
echo $?
[ $int1 -gt $int2 ]    #int1大于int2
echo $?
[ $int1 -ge $int2 ]   #int1大于或等于int2
echo $?
[ $int1 -lt $int2 ]   #int1小于int2
echo $?
[ $int1 -le $int2 ]   #int1小于等于int2
echo $?

echo 整数测试 [] 不能使用算数表达式,如果使用会出错
[ $[$int1+1] -eq $int2 ]
echo $?
[ $[$int1+1] == $int2 ]
echo $?
[ $[$int1+1] != $int2 ]
echo $?

[ $[$int1+1] > $int2 ]  #结果错
echo $?
# [ $[$int1+1] >= $int2 ]   #语法错
# echo $?
[ $[$int1+1] < $int2 ]  #结果错
echo $?
# [ $[$int1+1] <= $int2 ]   #语法错
# echo $?


#整数测试也可以使用let命令或双圆括号. 只能用于整数测试
# 相应的操作符为: ==    !=  >   >=  <   <=
# let和双圆括号中可以使用算数表达式,而中括号不能
# let和双圆括号中,操作符两边可以不留空格
echo 整数测试 let
let "$int1 <= $int2"
echo $?

echo 整数测试 '(())'
(($int1 <= $int2))
echo $?


#逻辑测试
echo 逻辑测试
[ $int1 -eq 1 -a $int2 -eq 2 ]
echo $?

#可以使用模式的逻辑测试 [[]]

[[ $int1 -eq 1 && $int2 -eq 2 ]]
echo $?
[[ $int1 -eq 1 || $int2 -eq 2 ]]
echo $?
[[ !$int1 -eq 1 ]]
echo $?

#检查空值
echo 检查空值
name=""
[ "$name" = "" ]
echo $?

[ "X$name" = "X" ]
echo $?

[ ! "$name" ]
echo $?


# 文件测试:文件是否存在,文件属性,访问权限等
echo 文件测试
[ -f 05condition.sh ]   #文件存在且是普通文件时,返回真
echo $?

[ -L 05condition.sh ]   #文件存在且是链接文件时,返回真
echo $?

[ -d 05condition.sh ]   #存在且是一个目录时,返回真

[ -e 05condition.sh ]   #存在时,返回真
echo $?

[ -s 05condition.sh ]   #存在且size大于0,返回真
echo $?

[ -r 05condition.sh ]   #存在且可读时,返回真
echo $?

[ -w 05condition.sh ]   #存在且可写时,返回真
echo $?

[ -x 05condition.sh ]   #存在且可执行时,返回真
echo $?


