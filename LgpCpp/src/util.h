/*
 * util.h
 *
 *  Created on: Mar 16, 2013
 *      Author: Vanessa Cañete, Nahuel Hernández
 */




float random() {
	//This will generate a number from 0.0 to 1.0, inclusive.
	float r = (float)rand()/(float)RAND_MAX;

	return r;
}

float random(float x) {
	//This will generate a number from 0.0 to some arbitrary float, x
	float r2 = (float)rand()/((float)RAND_MAX/x);

	return r2;
}

float random(float HI, float LO) {
	//This will generate a number from some arbitrary LO to some arbitrary HI

	float r3 = LO + (float) rand() / ((float) RAND_MAX / (HI - LO));

	return r3;
}

int randint(int HI, int LO) {
	int r = LO + rand() / (RAND_MAX / (HI - LO));

	return r;
}






