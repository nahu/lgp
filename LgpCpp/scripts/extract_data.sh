#!/bin/bash

cat $1 | grep generación > generaciones.txt
sed -e 's/generación\ \#//g' generaciones.txt -i
sed -e 's/  - best_init: /;/g' generaciones.txt -i
sed -e 's/  - best_end: /;/g' generaciones.txt -i
sed -e 's/  - diff: .*//g' generaciones.txt -i