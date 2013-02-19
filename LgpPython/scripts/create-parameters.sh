#!/bin/bash

PARAMETERS_DIR=../data/Parameters_
FILE_TO_COPY=../src/ParametersBase.py

for (( i=0; i < $1; i++ )); do
	cp $FILE_TO_COPY $PARAMETERS_DIR$i
done

printf "$1 Parameters creados...\n"
