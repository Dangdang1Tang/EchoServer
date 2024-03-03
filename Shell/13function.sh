#!/bin/bash

func()
{
    echo "Entering fun"
    echo "Exiting func"
}
func

func1()
{
    echo $#
    echo $1
    echo $2
}
func1 a b
