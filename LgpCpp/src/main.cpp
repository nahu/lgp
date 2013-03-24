//============================================================================
// Name        : main.cpp
// Author      : Vanessa Cañete, Nahuel Hernández
// Version     : 1
// Copyright   : Your copyright notice
// Description : Main Program for LGP
//============================================================================

#include <iostream>
#include <ctime>
#include <stdlib.h>

#include "parameters.h"
#include "util.h"

using namespace std;

int main() {
	srand((unsigned)time(0));
	init_parameters();


	cout << "!!!Hello World!!!" << endl;
	return 0;
}
