//============================================================================
// Name        : main.cpp
// Author      : Vanessa Cañete, Nahuel Hernández
// Version     : 1
// Copyright   : Your copyright notice
// Description : Main Program for LGP
//============================================================================

#include <iostream>
#include <fstream>
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

#include "parameters.h"
#include "util.h"
#include "program.h"

using namespace std;


int main() {
	srand((unsigned)time(0));
	Program::init_registers();

	return 0;
}
