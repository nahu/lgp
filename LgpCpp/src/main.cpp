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
#include <vector>
#include <algorithm>
#include <exception>

//#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include "parameters.h"
#include "util.h"
#include "program.h"
#include "individual.h"
#include "deme.h"


//using namespace std;


int main() {
	srand((unsigned)time(0));
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
	Individual g2(2, 1);
	Individual g1(1, 1);
	Individual * sister, * brother;

	//i.eval_fitness();
	//g1.print_individual();
	//g2.print_individual();
	//g1.program.print_list_int();
	//g2.program.print_list_int();
	std::cout<<"**Direccion sister main \t"<<&sister<<"\n";
	std::cout<<"Direccion sister.program main \t"<<&(sister->program)<<"\n";
	std::cout<<"Direccion brother main \t"<<&brother<<"\n";
	std::cout<<"Direccion brother.program main \t"<<&(brother->program)<<"\n";
	Individual::crossover(g1,g2,sister,brother);

	std::cout<<"DESDE MAIN"<<"\n";
	std::cout<<"Direccion sister main \t"<<&sister<<"\n";
	std::cout<<"Direccion sister.program main \t"<<&(sister->program)<<"\n";
	std::cout<<"Direccion brother main \t"<<&brother<<"\n";
	std::cout<<"Direccion brother.program main \t"<<&(brother->program)<<"\n";
	std::cout<<"Verificacion crossover sister"<<"\n"; //despues de esto da segmentation fault
	Program::print_list_int(sister->program->list_inst, sister->program->height);
	std::cout<<"Verificacion crossover brother"<<"\n";
	Program::print_list_int(brother->program->list_inst, brother->program->height);


	std::cout<<"Fin main"<<"\n";

	return 0;
}
