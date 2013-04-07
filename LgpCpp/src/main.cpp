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


using namespace std;


int main() {
	srand((unsigned)time(0));
	Program::init_registers();
		Individual g1(1, 1);
	Individual g2(2, 1);
	Individual * sister;
	Individual * brother;

	//i.eval_fitness();
	//g1.print_individual();
	//g2.print_individual();
	//g1.program.print_list_instructions();
	//g2.program.print_list_instructions();
	Individual::crossover(g1,g2,sister,brother);
	return 0;
}
