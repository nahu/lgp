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
#include <ctime>


#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <omp.h>
#include <time.h>


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

	clock_t main_begin, main_end, t_begin, t_end;
	timespec real_main_begin, real_main_end, real_t_begin, real_t_end;
	int hours, rest, minutes, seconds;

	main_begin = clock();
	clock_gettime(CLOCK_REALTIME, &real_main_begin);


	srand((unsigned)time(0));

	omp_set_num_threads(NUM_PROCESSORS);


	Program::init_registers();

	//Declaraciones
	Individual ** best_individuals_training;
	//Individual ** best_individuals_validation;

	std::string folder = "./resultados/";
	std::vector<int> posiciones (N - K);
	std::vector<double *> list_training_errors(DEMES), list_validation_errors(DEMES);
	double duration;

	std::string diff = get_current_time();


	folder = folder + diff;
	int exito = mkdir(folder.c_str(), 0777);
	if (exito < 0) {
		std::cout << "No se pudo crear la carpeta de resultados! \n";
		exit(EXIT_FAILURE);
	} else {
		std::cout << "Los resultados se guardaran en " << folder << std::endl;
	}

	//Se escriben los parametros
	parameters_to_file(folder + "/parametros.txt");

	int index = 0;
	for (int i = 0; i < N; i++) {
		if (CONFIG[i] == '0') {
			posiciones[index] = i;
			index++;
		}
	}

	for (std::vector<int>::iterator it = posiciones.begin(); it != posiciones.end(); ++it) {
		int i = *it;
		t_begin = clock();
		clock_gettime(CLOCK_REALTIME, &real_t_begin);

		std::cout << "*******************************************************************\n";
		std::cout << "---- Transformador " << i << "\n";
		std::cout << "*******************************************************************\n";

		Lgp * lgp = new Lgp(i, DEMES, POPULATION_SIZE, NUM_GENERATIONS);

		lgp->evolve();

		best_individuals_training = lgp->best_individuals_of_demes(TRAINING);
		//best_individuals_validation = lgp->best_individuals_of_demes(VALIDATION);

		t_end = clock();
		clock_gettime(CLOCK_REALTIME, &real_t_end);

		duration = (double) (t_end - t_begin) / CLOCKS_PER_SEC;

		hours = duration / 3600;
		rest = (int) duration % 3600;
		minutes = rest / 60;
		seconds = rest  % 60;

		std::cout << "Transformador " << i << "\n";
		std::cout << "Duración CPU: " << hours << ":" << minutes << ":" << seconds << " (" << duration << " seconds)\n";


		duration = (real_t_end.tv_sec - real_t_begin.tv_sec);

		hours = duration / 3600;
		rest = (int) duration % 3600;
		minutes = rest / 60;
		seconds = rest  % 60;

		std::cout << "Duración REAL: " << hours << ":" << minutes << ":" << seconds << " (" << duration << " seconds)\n";

		//****************************** Obtener listas de errores *****************************
		int chunks = DEMES / (NUM_PROCESSORS);
		#pragma omp parallel for schedule(static, chunks)
		for (int j = 0; j < DEMES; j++) {
			list_training_errors[j] = best_individuals_training[j]->eval_individual(TRAINING);
			list_validation_errors[j] = best_individuals_training[j]->eval_individual(VALIDATION);
		}



		//****************************** ESCRIBIR EN ARCHIVOS *****************************

		#pragma omp parallel sections // starts a new team
		{
			{
			std::stringstream f_errors_training;
			f_errors_training.str("");
			f_errors_training << folder << "/TRAINING-errores-TRAF" << i << "-G" << NUM_GENERATIONS << ".csv";
			errors_to_file(f_errors_training.str(), list_training_errors, TRAINING_LINES + 3);
			}

			#pragma omp section
			{
			std::stringstream f_errors_val;
			f_errors_val.str("");
			f_errors_val << folder << "/VALIDATION-errores-TRAF" << i << "-G" << NUM_GENERATIONS << ".csv";
			errors_to_file(f_errors_val.str(), list_validation_errors, VALIDATION_LINES + 3);
			}

			#pragma omp section
			{
			std::stringstream f_programs;
			f_programs.str("");
			f_programs << folder << "/TRAINING-programas-TRAF" << i << "-G" << NUM_GENERATIONS << ".txt";
			programs_to_file(f_programs.str(), best_individuals_training);
			}


			/*
			#pragma omp section
			{
			std::string f_programs_val = folder + "/VALIDATION-programas-TRAF" + transf.str() + "-G" + gen.str() + ".txt";
			programs_to_file(f_programs_val, best_individuals_validation);
			}*/
		}

		//Borrar cosas antes de la siguiente iteracion
		for (unsigned j = 0; j < DEMES; j++) {
			delete [] list_training_errors[j];
			//delete [] list_validation_errors[j];
		}

		delete [] best_individuals_training;
		delete lgp;
	}


	main_end = clock();
	clock_gettime(CLOCK_REALTIME, &real_main_end);
	duration = (double) (main_end - main_begin) / CLOCKS_PER_SEC;

	hours = duration / 3600;
	rest = (int) duration % 3600;
	minutes = rest / 60;
	seconds = rest  % 60;

	std::cout<<"\n\n--%%%%%%%%%% LGP/main \n";
	std::cout << "Duración CPU: " << hours << ":" << minutes << ":" << seconds << " (" << duration << " seconds)\n";

	duration = (real_main_end.tv_sec - real_main_begin.tv_sec);

	hours = duration / 3600;
	rest = (int) duration % 3600;
	minutes = rest / 60;
	seconds = rest  % 60;

	std::cout << "Duración REAL: " << hours << ":" << minutes << ":" << seconds << " (" << duration << " seconds)\n";


	return 0;
}
