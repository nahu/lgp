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
//todo: crear contadores globales de operadores geneticos y migracion.
int global_cant_crossover = 0;
int global_cant_migracion = 0;

int global_cant_macro = 0;
int global_cant_macro_del = 0;
int global_cant_macro_ins = 0;

int global_cant_micro = 0;
int global_cant_micro_reg = 0;
int global_cant_micro_const = 0;
int global_cant_micro_ope = 0;

void init_individual_counters(){
	std::cout << "--- ----------------------- ---" << std::endl;
	std::cout << "Inicializacion de contadores Individuales" << std::endl;
	std::cout << "--- ----------------------- ---" << std::endl;
	for (int i = 0; i<NUM_PROCESSORS; i++){
		Individual::cant_macro[i] = 0;
		Individual::cant_macro_del[i] = 0;
		Individual::cant_macro_ins[i] = 0;
		Individual::cant_micro[i] = 0;
		Individual::cant_micro_reg[i] = 0;
		Individual::cant_micro_const[i] = 0;
		Individual::cant_micro_ope[i] = 0;
		Individual::cant_crossover[i] = 0;
	}
}
void init_Lgp_counters(){
	std::cout << "--- ----------------------- ---" << std::endl;
	std::cout << "Inicializacion de contadores por transformador" << std::endl;
	std::cout << "--- ----------------------- ---" << std::endl;
	for (int i = 0; i<NUM_PROCESSORS; i++){
		Lgp::cant_migracion[i] = 0;
	}
	init_individual_counters();
}

void init_global_counters(){
	/*
	 * Los contadores globales son por todos los transformadores
	 * Tienen la sumatoria de los contadores locales (Inicializados en Lgp::evolve())
	 * La funcion Lgp::evolve() es llamada por cada transformador.
	 */
	std::cout << "--- ----------------------- ---" << std::endl;
	std::cout << "Inicializacion de contadores globales" << std::endl;
	std::cout << "--- ----------------------- ---" << std::endl;
	global_cant_crossover = 0;
	global_cant_migracion = 0;

	global_cant_macro = 0;
	global_cant_macro_del = 0;
	global_cant_macro_ins = 0;

	global_cant_micro = 0;
	global_cant_micro_reg = 0;
	global_cant_micro_const = 0;
	global_cant_micro_ope = 0;
	std::cout << "*** ----------------------- ***" << std::endl;
}
void update_counters(){
	/*
	 * Cada deme suma sus operaciones geneticas en una posicion del vector de cant_*
	 * Para obtener los datos por programa se totaliza y se suma a las variables globales.
	 */
	std::cout << "****------------------------------------------------------------****\n";
	std::cout << "Actualizacion de contadores"<<std::endl;
	std::cout << "*******************************************************************\n";
	global_cant_crossover += get_counters_sum(Individual::cant_crossover);
	global_cant_migracion += get_counters_sum(Lgp::cant_migracion);

	global_cant_macro += get_counters_sum(Individual::cant_macro);
	global_cant_macro_del += get_counters_sum(Individual::cant_macro_del);
	global_cant_macro_ins += get_counters_sum(Individual::cant_macro_ins);

	global_cant_micro += get_counters_sum(Individual::cant_micro);
	global_cant_micro_reg += get_counters_sum(Individual::cant_micro_reg);
	global_cant_micro_const += get_counters_sum(Individual::cant_micro_const);
	global_cant_micro_ope += get_counters_sum(Individual::cant_micro_ope);
	std::cout << "****------------------------------------------------------------****\n";
}
/*
 * ******************************************************************************************
 *  									MAIN
 * ******************************************************************************************
 */

int main(int argc, char ** argv) {
	//std::string diff = get_current_time();
	//folder = folder + diff;
	std::string folder_orig;

	if (argc == 1) {
		std::cout << "No se ha recibido ningun parametro para el programa." << std::endl;
	} else {
		std::cout <<"Se recibio argumento" << std::string (argv[1]) << std::endl;
		folder_orig = std::string (argv[1]);
	}

	srand((unsigned)time(0));
	//omp_set_num_threads(NUM_PROCESSORS);
	Program::init_registers();
	int primero;
	for (int p = 0; p < CNT_PRUEBAS; p++) {
		primero = 1;

		init_global_counters();

		std::stringstream st; st << p;
		std::string folder = folder_orig + st.str() + "/";

		//Declaraciones
		clock_t main_begin, main_end, t_begin, t_end;
		timespec real_main_begin, real_main_end, real_t_begin, real_t_end;
		Individual ** best_individuals_training;
		//std::vector<int> posiciones (N_K);
		std::vector<int> posiciones (1); //sacar para procesar todos

		std::vector<double *> list_training_errors(DEMES), list_validation_errors(DEMES);
		double duration;

		main_begin = clock();
		clock_gettime(CLOCK_REALTIME, &real_main_begin);

		//Creacion de carpeta de resultados
		int exito = mkdir(folder.c_str(), 0777);
		if (exito < 0) {
			std::cout << "No se pudo crear la carpeta de resultados! \n" << folder << "\n";
			exit(EXIT_FAILURE);
		} else {
			std::cout << "Los resultados se guardaran en " << folder << std::endl;
		}

		//Se escriben los parametros
		parameters_to_file(folder + "/parametros.txt");
		/*
		int index = 0;
		for (int i = 0; i < N; i++) {
			if (CONFIG[i] == '0') {
				posiciones[index] = i;
				index++;
			}
		}
		*/
		posiciones[0] = CONFIG_POSITION; //sacar para procesar todos
		//Aca se agregan los mejores individuos por cada trafo
		Individual * best_global = new Individual[posiciones.size()];
		int global_pos = 0;
		std::vector<double *> list_global_errors(posiciones.size());

		//****************************** PROCESAMIENTO DE TRANSFORMADORES *****************************
		for (std::vector<int>::iterator it = posiciones.begin(); it != posiciones.end(); ++it) {

			init_Lgp_counters();

			int i = *it;
			t_begin = clock();
			clock_gettime(CLOCK_REALTIME, &real_t_begin);

			std::cout << "*******************************************************************\n";
			std::cout << "---- Transformador " << i << "\n";
			std::cout << "*******************************************************************\n";

			Lgp * lgp = new Lgp(i, DEMES, POPULATION_SIZE, NUM_GENERATIONS);

			Individual  best_init = *(lgp->best_individual_in_training());

			lgp->evolve();

			update_counters();

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

				#pragma omp section
				{
					std::stringstream posicion;
					posicion<<i;
					trafo_counters_to_file(posicion.str(), folder, primero, &best_init, &best_global[global_pos-1]);
				}

				#pragma omp section
				{
					std::stringstream posicion;
					posicion<<i;
					errors_generation_to_file(posicion.str(), folder, TRAINING);
				}

				#pragma omp section
				{
					std::stringstream posicion;
					posicion<<i;
					errors_generation_to_file(posicion.str(), folder, VALIDATION);
				}
			}

			//Borrar cosas antes de la siguiente iteracion
			for (unsigned j = 0; j < DEMES; j++) {
				delete [] list_training_errors[j];
				//delete [] list_validation_errors[j];
			}

			delete [] best_individuals_training;
			delete lgp;
			primero = 0;
		}//End for de transformadores

		main_end = clock();
		clock_gettime(CLOCK_REALTIME, &real_main_end);
		duration = (double) (main_end - main_begin) / CLOCKS_PER_SEC;
		write_duration("\n\n--%%%%%%%%%% LGP/main \n\n Duración CPU: ", duration);
		duration = (real_main_end.tv_sec - real_main_begin.tv_sec);
		write_duration("Duración REAL: ", duration);
		std::cout << "------- antes de escribir resultados -----------";
		save_global_results(best_global, posiciones.size(), folder);

		global_counters_to_file( folder, global_cant_crossover, global_cant_migracion, global_cant_macro,
				global_cant_macro_del, global_cant_macro_ins, global_cant_micro, global_cant_micro_reg,
				global_cant_micro_const, global_cant_micro_ope, posiciones.size());
		delete [] best_global;
	}//End for CTN_PRUEBAS

	return 0;
}

