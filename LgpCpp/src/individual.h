/*
 * individual.h
 *
 *  Created on: Mar 16, 2013
 *      Author: Vanessa Cañete, Nahuel Hernández
 */

class Individual {
public:
	Individual(int index, int config_position);
	Program program;
	double fitness;
	double error;
	double sigma; //era dev
	int index;
	int config_position;//ver si hace falta
	int n_eff;
	bool evaluated;
	//pasar todo lo que esta en la clase individuo
	//poner acá los operadores genéticos
};
