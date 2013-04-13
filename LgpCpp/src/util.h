/*
 * util.h
 *
 *  Created on: Mar 16, 2013
 *      Author: Vanessa Cañete, Nahuel Hernández
 */


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
	if (HI == LO){
		return HI;
	}

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

void imprimir_matriz (double ** matriz, int n, int m){
	for (int current_sample = 0; current_sample < n; current_sample++ )	{
		for (int current_trafo = 0; current_trafo < m; current_trafo++ ) {
			printf("%.6f \t", matriz[current_sample][current_trafo]);
		}
		printf("\n");
	}
	printf("\n");
}

double ** get_matrix_from_file() {
	std::string medida;
	int current_trafo = 0;
	int current_sample = 0;

	double ** data = new double*[LINES];

	for (current_sample = 0; current_sample < LINES; current_sample++ ) {
		data[current_sample] = new double[N];
	}

	//Apertura del archivo
	std::ifstream myfile;
	myfile.open(FILE_NAME, std::ios::in);
	getline(myfile, medida, '\n' );
	getline(myfile, medida, '\n' );

	if (myfile.is_open()) {
		for (current_sample = 0; current_sample < LINES; current_sample++ ) {
			char * c_medida;
			getline(myfile, medida);
			c_medida = (char*) medida.c_str();
			char * ptr = strtok(c_medida, ";");
			for (current_trafo = 0; current_trafo < N; current_trafo++ ){
				//guardar en una posicion de la matriz
				data[current_sample][current_trafo] = atof(ptr);
				//std::cout<<data[current_sample][current_trafo]<<"\t";
				ptr = strtok(0, ";");
			}
			//std::cout<<"\n";

		}
		//imprimir_matriz(data,LINES,N);
		myfile.close();

	}
	return data;
}



