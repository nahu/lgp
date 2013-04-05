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
	Individual i(1, 1);

	i.eval_fitness();
	i.print_individual();
	//double hola = log10(abs(-0.866767));
	//std::cout << "hola: " << hola << "\n";
	return 0;
}
