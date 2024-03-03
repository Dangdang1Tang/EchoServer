#!/bin/bash

# select是个无限循环,因此要记住用break命令退出循环,或用exit命令终止循环,也可以按ctrl+c退出循环.
# select经常和case联合使用,
# 与for循环类似,可以省略in list,此时使用位置参量

select var in Dogs Cats Birds
do
    case $var in
        Dogs)
            echo Dogs are my favorite pet
            break
        ;;
        Cats)
            echo Cats are my favorite pet
        ;;
        Birds)
            echo Birds are my favorite pet
        ;;
        *)
            echo none are my favorite pet
        ;;
    esac
done
