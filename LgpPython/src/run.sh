#!/bin/bash

PARAMETERS=(../data/Parameters*)

for (( i=0; i < ${#PARAMETERS[@]}; i++ )); do
	p=${PARAMETERS[$i]}
	printf "Usando $p... salida en salida-$i.txt\n"
	rm Parameters.py
	ln -s $p Parameters.py
	python LgpMain.py > salida-$i.txt
done

rm Parameters.py
ln -s ParametersBase.py Parameters.py
