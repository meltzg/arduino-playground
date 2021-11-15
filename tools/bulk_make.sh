#!/bin/bash

while true
do
    read -p "Press any key to run make $*"
    make $*
done
