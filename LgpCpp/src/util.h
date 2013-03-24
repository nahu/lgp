/*
 * util.h
 *
 *  Created on: Mar 16, 2013
 *      Author: Vanessa Cañete, Nahuel Hernández
 */



#include <stdlib.h>

float _random() {
	//This will generate a number from 0.0 to 1.0, inclusive.
	float r = (float) rand() / (float) RAND_MAX;

	return r;
}

float _random(float x) {
	//This will generate a number from 0.0 to some arbitrary float, x
	float r2 = (float)rand()/((float)RAND_MAX/x);

	return r2;
}

float randfloat(float HI, float LO) {
	//This will generate a number from some arbitrary LO to some arbitrary HI

	float r3 = LO + (float) rand() / ((float) RAND_MAX / (HI - LO));

	return r3;
}

int randint(int HI, int LO) {
	int r = LO + rand() / (RAND_MAX / (HI - LO));

	return r;
}


bool random_flip_coin(float p) {
/*
 *  Returns True with the *p* probability. If the *p* is 1.0,
 *  the function will always return True, or if is 0.0, the
 *  function will return always False.
 */
	if (p == 1.0) {
		return true;
	} else if (p == 0.0) {
		return false;
	} else if (_random() <= p) {
		return true;
	} else {
		return false;
	}
}



