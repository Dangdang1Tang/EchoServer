#!/bin/bash
echo hello
echo "hello"
echo 'hello'
var=aa
echo ${var}b
echo -n "hello world"   #-n表示输出内容后不再输出\n
echo -e "\r"            #-e表示可以使用转义字符

# 单引号 忽略所有特殊字符
# 双引号 忽略大部分特殊字符 （$等字符除外）
# 反引号 命令替换（将一个命令的标准输出插入到命令的任何位置）
# $() 同上
# 命令替换可以嵌套,如果使用反引号,则内部的反引号必须用反斜杠来转义.

echo `pwd`
echo $(pwd)
echo `basename \`pwd\``
echo $(basename $(pwd))
