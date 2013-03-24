/*
 * individual.h
 *
 *  Created on: Mar 16, 2013
 *      Author: Vanessa Cañete, Nahuel Hernández
 */

class Individual {
public:
	Individual(int, int);
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

Individual::Individual(int _index, int _config_position) {
	program = new Program;
	fitness = 0.0;
	error = 0.0;
	sigma = 0.0;
	index = _index;
	config_position = _config_position;
	n_eff = 1;
	evaluated = false;
}

Individual::~Individual() {
	delete program;
}
