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

#include "parameters.h"
#include "util.h"
#include "program.h"
#include "individual.h"
#include "deme.h"
#include "lgp.h"
//using namespace std;

int main() {
	clock_t main_begin, main_end, t_begin, t_end;
	main_begin = clock();

	//srand((unsigned)time(0));
	//omp_set_num_threads(NUM_PROCESSORS);

	Program::init_registers();

	//Declaraciones
	std::vector<Individual> best_individuals_training, best_individuals_validation;
	std::string folder = "./resultados";
	std::vector<int> posiciones (N - K);
	std::vector<std::vector<double> > list_training_errors, list_validation_errors;
	double duration;

/*
	Individual * g2 = new Individual;
	Individual * g1 = new Individual;
	g1->create_new_individual(1);
	g2->create_new_individual(1);

	Individual * sister, * brother;
	sister = new Individual();
	brother = new Individual;

	std::cout<<"Antes del crossover \n";
	std::cout<<"*** Main: Lista g1.\n";
	Program::print_list_int(g1->program->list_inst, g1->program->height);

	std::cout<<"*** Main: Lista g2.\n";
	Program::print_list_int(g2->program->list_inst, g2->program->height);

	//i.eval_fitness();


	//while(1){

	std::cout<<"\n\n*** Main: Lista original.\n";
	Program::print_list_int(g2->program->list_inst, g2->program->height);
	g2->micro_mutation();
	std::cout<<"*** Main: Lista mutada 1.\n";
	Program::print_list_int(g2->program->list_inst, g2->program->height);

	//}

	//g2.micro_mutation();
	//std::cout<<"*** Main: Lista mutada 2.\n";
	//Program::print_list_int(g2.program->list_inst, g2.program->height);


*/

	//Se escriben los parametros

	parameters_to_file("./resultados/parametros.txt");

	int index = 0;
	for (int i = 0; i < N; i++) {
		if (CONFIG[i] == '0') {
			posiciones[index] = i;
			std::cout << "i: " <<  posiciones[index] << "\n";
			index++;
		}
	}

	//for (std::vector<int>::iterator it = positions.begin(); it != positions.end(); ++it) {
		int i = 1;//*it;
		t_begin = clock();

		std::cout << "************************************************************************************\n";
		std::cout << "---- Transformador " << i << "\n";
		std::cout << "************************************************************************************\n";

		Lgp * lgp = new Lgp(i, DEMES, POPULATION_SIZE, NUM_GENERATIONS);

		lgp->evolve();
		std::cout << "Despues de evolve\n";

		best_individuals_training = lgp->best_individuals_of_demes(TRAINING);
		best_individuals_validation = lgp->best_individuals_of_demes(VALIDATION);


		t_end = clock();

		duration = (double) (t_end - t_begin) / CLOCKS_PER_SEC;
		std::cout << "++++++++Transf. " << i << "Duracion: " << duration << "\n";
		/*
		//****************************** Obtener lista de errores *****************************


		for (int j = 0; j < best_individuals_training.size(); j++) {
			list_training_errors.push_back(best_individuals_training.at(j).eval_individual(TRAINING));
		}
		for (int j = 0; j < best_individuals_validation.size(); j++) {
			list_validation_errors.push_back(best_individuals_validation.at(j).eval_individual(VALIDATION));
		}

		//****************************** CALCULAR SUMA & PROM ***************************** *
		for (int j = 0; j < list_training_errors.size(); j++){
			double sum, prom = 0.0;
			for (int k = 0; k < list_training_errors.at(j).size(); k++){
				sum += list_training_errors.at(j).at(k);
			}
			prom = sum / list_training_errors.at(j).size();
			list_training_errors.at(j).push_back(sum); //sumatoria
			list_training_errors.at(j).push_back(prom); //error_promedio
		}

		//****************************** CALCULAR SUMA & PROM *****************************

		for (int j = 0; j < list_validation_errors.size(); j++){
			double sum, prom = 0.0;
			for (int k = 0; k < list_validation_errors.at(j).size(); k++){
				sum += list_validation_errors.at(j).at(k);
			}
			prom = sum / list_validation_errors.at(j).size();
			list_validation_errors.at(j).push_back(sum); //sumatoria
			list_validation_errors.at(j).push_back(prom); //error_promedio
			list_validation_errors.at(j).push_back(i);//posicion
		}


		//****************************** ESCRIBIR EN ARCHIVOS *****************************
		std::cout<<"Antes de escribir en archivos. ";
		std::stringstream transf, gen; transf<<i; gen<<lgp->generation;
		std::string f_errors_training = folder + "/TRAINING-errores-TRAF" + transf.str() + "-G" + gen.str() + ".csv";
		errors_to_file(f_errors_training, list_training_errors);
		std::string f_errors_val = folder + "/VALIDATION-errores-TRAF" + transf.str() + "-G" + gen.str() + ".csv";
		errors_to_file(f_errors_val, list_validation_errors);

		std::string f_programs = folder + "/programas-TRAF" + transf.str() + "-G" + gen.str() + ".txt";
		//todo programs_to_file(f_programs, best_individuals)

		 */
		delete lgp;
	//}

	main_end = clock();
	duration = (double) (main_end - main_begin) / CLOCKS_PER_SEC;
	std::cout<<"\n\n%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Duracion LGP/main : " << duration << "\n";

	return 0;
}
