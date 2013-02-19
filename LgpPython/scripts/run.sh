#!/bin/bash

PARAMETERS=(../data/Parameters*)
FILE=../src/Parameters.py
OUT_DIR=../resultados/$1/
ORIGINAL=../src/ParametersBase.py

mkdir $OUT_DIR
 
for (( i=0; i < ${#PARAMETERS[@]}; i++ )); do
	p=${PARAMETERS[$i]}
	mkdir $OUT_DIR$i
	printf "Usando $p... salida en $OUT_DIR$i/salida-$i.txt\n"
	rm $FILE
	ln -s $p $FILE
	python ../src/LgpMain.py $OUT_DIR$i/ > $OUT_DIR$i/salida-$i.txt
done

rm $FILE
ln -s $ORIGINAL $FILE
