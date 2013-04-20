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

//#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <omp.h>

#include "parameters.h"
#include "util.h"
#include "program.h"
#include "individual.h"
#include "deme.h"
#include "lgp.h"


//using namespace std;


int main() {
	srand((unsigned)time(0));
	omp_set_num_threads(NUM_PROCESSORS);
	Program::init_registers();

	//Individual i(1, 1);
	//i.eval_fitness();
	//i.print_individual();
	/*
	Deme * d = new Deme(3, 1);

	for (int i = 0; i < d->deme_size; i++) {
		d->list_ind[i].print_individual();
	}
	*/


	Individual * g2 = new Individual;
	Individual * g1 = new Individual;
	g1->create_new_individual(1);
	g2->create_new_individual(1);

	/*
	Individual * sister, * brother;
	sister = new Individual();
	brother = new Individual;

	std::cout<<"Antes del crossover \n";
	std::cout<<"*** Main: Lista g1.\n";
	Program::print_list_int(g1->program->list_inst, g1->program->height);

	std::cout<<"*** Main: Lista g2.\n";
	Program::print_list_int(g2->program->list_inst, g2->program->height);

	//i.eval_fitness();
	Individual::crossover(g1, g2);
	std::cout<<"Despues crossover \n";
	std::cout<<"*** Main: Lista g1.\n";
	Program::print_list_int(g1->program->list_inst, g1->program->height);

	std::cout<<"*** Main: Lista g2.\n";
	Program::print_list_int(g2->program->list_inst, g2->program->height);
	*/

	//while(1){

	std::cout<<"*** Main: Lista original.\n";
	Program::print_list_int(g2->program->list_inst, g2->program->height);
	g2->macro_mutation();
	std::cout<<"*** Main: Lista mutada 1.\n";
	Program::print_list_int(g2->program->list_inst, g2->program->height);
	//}
	/*g2.micro_mutation();
	std::cout<<"*** Main: Lista mutada 2.\n";
	Program::print_list_int(g2.program->list_inst, g2.program->height);
	*/


	return 0;
}
