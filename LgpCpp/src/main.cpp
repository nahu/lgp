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



int main(int argc, char ** argv) {
	std::string folder_orig = "./resultados/hola/1";
	//std::string diff = get_current_time();
	//folder = folder + diff;

	if (argc<1){
		std::cout<< "No se ha recibido ningun parametro para el programa."<<std::endl;
	}else{
		std::string folder_orig(argv[0]);
	}

	for (int p = 0; p < CNT_PRUEBAS; p++) {
		std::stringstream st; st << p;
		std::string folder = folder_orig + "/" + st.str() + "/"; //./resultados/hola/1/p/

		//Declaraciones
		clock_t main_begin, main_end, t_begin, t_end;
		timespec real_main_begin, real_main_end, real_t_begin, real_t_end;
		Individual ** best_individuals_training;
		std::vector<int> posiciones (N - K);
		std::vector<double *> list_training_errors(DEMES), list_validation_errors(DEMES);
		double duration;



		main_begin = clock();
		clock_gettime(CLOCK_REALTIME, &real_main_begin);
		srand((unsigned)time(0));
		omp_set_num_threads(NUM_PROCESSORS);

		Program::init_registers();

		//Creacion de carpeta de resultados
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
		//Aca se agregan los mejores individuos por cada trafo
		Individual * best_global = new Individual[posiciones.size()];
		int global_pos = 0;
		std::vector<double *>  list_global_errors(posiciones.size());

		//****************************** PROCESAMIENTO DE TRANSFORMADORES *****************************
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
			best_global[global_pos++] = *(lgp->best_individual_in_training());

			t_end = clock();
			clock_gettime(CLOCK_REALTIME, &real_t_end);

			duration = (double) (t_end - t_begin) / CLOCKS_PER_SEC;
			st.clear(); st << "Transformador " << i << "\n";
			write_duration(st.str() + "Duración CPU: ", duration);


			duration = (real_t_end.tv_sec - real_t_begin.tv_sec);
			st.clear(); st << "Transformador " << i << "\n";
			write_duration(st.str() +  "Duración REAL: ", duration);

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

			}

			//Borrar cosas antes de la siguiente iteracion
			for (unsigned j = 0; j < DEMES; j++) {
				delete [] list_training_errors[j];
				//delete [] list_validation_errors[j];
			}

			delete [] best_individuals_training;
			delete lgp;

		}//End for de transformadores

		main_end = clock();
		clock_gettime(CLOCK_REALTIME, &real_main_end);
		duration = (double) (main_end - main_begin) / CLOCKS_PER_SEC;
		write_duration("\n\n--%%%%%%%%%% LGP/main \n\n Duración CPU: ", duration);
		duration = (real_main_end.tv_sec - real_main_begin.tv_sec);
		write_duration("Duración REAL: ", duration);

		//Se escriben los resultados de esta prueba
		save_global_results(best_global, folder);

	}//End for CTN_PRUEBAS

	return 0;
}
