#!/bin/bash

PARAMETERS=(../data/parameters*)
FILE=../src/parameters.h
OUT_DIR=./resultados/$1/
ORIGINAL=../src/parameters_base.h


#mkdir .$OUT_DIR
#printf "Creado .$OUT_DIR\n"

for (( i=0; i < ${#PARAMETERS[@]}; i++ )); do
	p=${PARAMETERS[$i]}
	
	rm $FILE
	ln -s $p $FILE
	
	cd ..
	#mkdir $OUT_DIR$i
	#printf "Usando - $p - salida en $OUT_DIR$i/salida-$i.txt\n"
	printf "Compilando...\n"
	g++ ./src/main.cpp -Wall -O3 -g3 -p -fmessage-length=0  -MMD -MP -fopenmp -lrt -w -o ./debug/lgp

	#printf "Depurando...\n"
	#gdb ./OpenMP/LgpCpp $OUT_DIR$i/ > $OUT_DIR$i/salida-$i.txt
	cd ./scripts
done

rm $FILE
ln -s $ORIGINAL $FILE
printf "FIN\n"
