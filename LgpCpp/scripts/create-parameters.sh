#!/bin/bash

PARAMETERS_DIR=../data/parameters_
FILE_TO_COPY=../src/parameters_base.h

rm $PARAMETERS_DIR*
for (( i=0; i < $1; i++ )); do
	cp $FILE_TO_COPY $PARAMETERS_DIR$i.h
done

printf "$1 Parameters creados...\n"
