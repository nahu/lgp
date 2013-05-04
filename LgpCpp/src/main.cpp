//============================================================================
// Name        : main.cpp
// Author      : Vanessa Cañete, Nahuel Hernández
// Version     : 1
// Copyright   : Your copyright notice
// Description : Main Program for LGP
//============================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>
#include <cmath>
#include <set>
//#include <new>
#include <vector>
#include <algorithm>
#include <exception>
#include <sstream>

//#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <omp.h>
#include <sys/time.h>
#include <ctime>

#include "parameters.h"
#include "util.h"
#include "program.h"
#include "individual.h"
#include "deme.h"
#include "lgp.h"
#include "print_file_util.h"
//using namespace std;



int main() {
	/*int count = 0;
	while(true) {
		int hola = randint(0,5 - 1);
		count++;
		//std::cout<< " " << hola << std::endl;
		if (hola == 5) {
			std::cout<< "************************** count " << count << std::endl;
		}
	}*/
	//std::cout<<"Prueba Pragma de deme_evolve "<< std::endl;
	clock_t main_begin, main_end, t_begin, t_end;
	main_begin = clock();
	srand((unsigned)time(0));

	//srand(7);

	//int threads;

	//omp_set_num_threads(NUM_PROCESSORS);
	//threads = omp_get_num_threads();
	//std::cout << "threads out: " << threads << "\n";


	/*
	#pragma omp parallel
	{
		threads = omp_get_num_threads();
		std::cout << "threads in: " << threads << "\n";
	}*/


	Program::init_registers();

	//Declaraciones
	std::vector<Individual> best_individuals_training (DEMES), best_individuals_validation(DEMES);
	std::string folder = "./resultados/";
	std::vector<int> posiciones (N - K);
	std::vector<std::vector<double> > list_training_errors(DEMES), list_validation_errors(DEMES);
	double duration;

	std::string diff = get_current_time();
	folder = folder + diff + "/";
	int exito = mkdir(folder.c_str(), 0777);
	if (exito<0){
		std::cout<< "No se pudo crear la carpeta de resultados! \n";
		exit(EXIT_FAILURE);
	}else{
		std::cout<< "Los resultados se guardaran en "<< folder <<std::endl;
	}

	//Se escriben los parametros
	parameters_to_file(folder + "parametros.txt");

	int index = 0;
	for (int i = 0; i < N; i++) {
		if (CONFIG[i] == '0') {
			posiciones[index] = i;
			//std::cout << "i: " <<  posiciones[index] << "\n";
			index++;
		}
	}

	for (std::vector<int>::iterator it = posiciones.begin(); it != posiciones.end(); ++it) {
		int i = *it;
		t_begin = clock();

		std::cout << "************************************************************************************\n";
		std::cout << "---- Transformador " << i << "\n";
		std::cout << "************************************************************************************\n";

		Lgp * lgp = new Lgp(i, DEMES, POPULATION_SIZE, NUM_GENERATIONS);

		lgp->evolve();

		best_individuals_training = lgp->best_individuals_of_demes(TRAINING);
		best_individuals_validation = lgp->best_individuals_of_demes(VALIDATION);

		t_end = clock();

		duration = (double) (t_end - t_begin) / CLOCKS_PER_SEC;
		std::cout << "Transformador " << i << " - Duracion: " << duration << "\n";


		//****************************** Obtener listas de errores *****************************
		for (int j = 0; j < best_individuals_training.size(); j++) {
			list_training_errors[j] = best_individuals_training.at(j).eval_individual(TRAINING);
		}

		for (int j = 0; j < best_individuals_validation.size(); j++) {
			list_validation_errors[j] = best_individuals_validation.at(j).eval_individual(VALIDATION);
		}

		//****************************** CALCULAR SUMA & PROM ***************************** *

		for (int j = 0; j < list_training_errors.size(); j++) {
			double sum, prom;
			sum = 0.0;

			for (int k = 0; k < list_training_errors.at(j).size(); k++) {
				sum += list_training_errors[j][k];
			}

			prom = sum / list_training_errors.at(j).size();
			list_training_errors.at(j).push_back(sum); //sumatoria
			list_training_errors.at(j).push_back(prom); //error_promedio
			list_training_errors.at(j).push_back(i);//posicion
		}
		//****************************** CALCULAR SUMA & PROM *****************************

		for (int j = 0; j < list_validation_errors.size(); j++) {
			double sum, prom;
			sum = 0.0;

			for (int k = 0; k < list_validation_errors.at(j).size(); k++) {
				sum += list_validation_errors[j][k];
			}

			prom = sum / list_validation_errors.at(j).size();

			list_validation_errors.at(j).push_back(sum); //sumatoria
			list_validation_errors.at(j).push_back(prom); //error_promedio
			list_validation_errors.at(j).push_back(i);//posicion
		}


		//****************************** ESCRIBIR EN ARCHIVOS *****************************
		std::stringstream transf, gen;
		transf << i;
		gen << lgp->generation;

		std::string f_errors_training = folder + "/TRAINING-errores-TRAF" + transf.str() + "-G" + gen.str() + ".csv";
		errors_to_file(f_errors_training, list_training_errors);

		std::string f_errors_val = folder + "/VALIDATION-errores-TRAF" + transf.str() + "-G" + gen.str() + ".csv";
		errors_to_file(f_errors_val, list_validation_errors);

		std::string f_programs = folder + "/TRAINING-programas-TRAF" + transf.str() + "-G" + gen.str() + ".txt";
		programs_to_file(f_programs, best_individuals_training);

		f_programs = folder + "/VALIDATION-programas-TRAF" + transf.str() + "-G" + gen.str() + ".txt";
		programs_to_file(f_programs, best_individuals_validation);



		delete lgp;
	}


	main_end = clock();
	duration = (double) (main_end - main_begin) / CLOCKS_PER_SEC;
	std::cout<<"\n\n--Duracion LGP/main : " << duration << "\n";
	//days = time / 86400 hours = (time / 3600) - (days * 24) minutes = (time / 60) - (days * 1440) - (hours * 60) seconds = time mod 60
	return 0;
}
