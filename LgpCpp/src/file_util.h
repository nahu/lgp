/*
 * individual.h
 *
 *  Created on: Mar 16, 2013
 *      Author: Vanessa Cañete, Nahuel Hernández
 */
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

double ** get_matrix_from_file() {
	string medida;
	int current_trafo = 0;
	int current_sample = 0;

	double ** data = new double*[LINES];
	for (current_sample = 0; current_sample < LINES; current_sample++ )
	{
		data[current_sample] = new double[N];
	}
	//Apertura del archivo
	ifstream myfile;
	myfile.open("../data/Datos60.txt", ios::in);

	if (myfile.is_open())
	{
		for (current_sample = 0; current_sample < LINES; current_sample++ )
		{
			for (current_trafo = 0; current_trafo < N; current_trafo++ ){
				//Leer hasta el primer ";"
				getline( myfile, medida, ';' );
				//guardar en una posicion de la matriz
				data[current_sample][current_trafo]= atof(medida.c_str());
			}
		}
		myfile.close();
	}
	return data;
}

void imprimir_matriz (double ** matriz){
	int current_trafo = 0;
	int current_sample = 0;
	for (current_sample = 0; current_sample < LINES; current_sample++ )	{
		for (current_trafo = 0; current_trafo < N; current_trafo++ ){
			printf("%.6f \t", matriz[current_sample][current_trafo]);
		}
		printf("\n");
	}
	printf("\n");
}
