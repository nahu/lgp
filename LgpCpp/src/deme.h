/*
 * deme.h
 *
 *  Created on: Mar 16, 2013
 *      Author: Vanessa Cañete, Nahuel Hernández
 */


class Deme { //esto es population
public:
	Deme(int deme_size);
	Individual *list_ind;
	int deme_size;
	//los métodos relacionados a population
	void deme_evolve();

};
