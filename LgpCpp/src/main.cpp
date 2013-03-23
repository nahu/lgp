//============================================================================
// Name        : main.cpp
// Author      : Vanessa Cañete, Nahuel Hernández
// Version     : 1
// Copyright   : Your copyright notice
// Description : Main Program for LGP
//============================================================================

#include <iostream>
#include <ctime>
#include <stdlib.h>
#include "parameters.h"
#include <string.h>
//#include "util.h"

#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "file_util.h"
using namespace std;

int main() {
	srand((unsigned)time(0));
	//Se crea la matriz -- ver si corresponde crear en parameters.
	double ** data = new double*[LINES];
	// Se crea una linea por muestra.
	int current_sample = 0;
	for (current_sample = 0; current_sample < LINES; current_sample++ )
	{
		data[current_sample] = new double[N];
	}
	//se carga la matriz desde el archivo
	data = get_matrix_from_file();
	//se imprime la matriz
	imprimir_matriz(data);

	return 0;
}
