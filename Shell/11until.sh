#!/bin/bash

counter=0
until   [ $counter -eq 3 ]
do
    echo AAA
    sleep 1
    counter=$[$counter+1]
done

