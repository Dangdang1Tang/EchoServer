#!/bin/bash


case $1 in
A)
    echo this is A
    ;;
B|b)
    echo this B or b
    ;;
*)
    echo others
    ;;
esac

