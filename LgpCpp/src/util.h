/*
 * util.h
 *
 *  Created on: Mar 16, 2013
 *      Author: Vanessa Cañete, Nahuel Hernández
 */
#include <iomanip>
float _random() {
	//This will generate a number from 0.0 to 1.0, inclusive.
	float r = (float) rand() / (float) RAND_MAX;

	return r;
}

float _random(float x) {
	//This will generate a number from 0.0 to some arbitrary float, x
	float r2 = (float) rand() / ((float) RAND_MAX / x);

	return r2;
}

float randfloat(float LO, float HI) {
	//This will generate a number from some arbitrary LO to some arbitrary HI
	if (HI == LO) {
		return HI;
	}

	float r3 = LO + (float) rand() / ((float) RAND_MAX / (HI - LO));
	return r3;
}

int randint(int LO, int HI) {
	if (HI == LO) {
		return HI;
	}


	int r = LO + (rand() % (HI - LO + 1) );

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

void imprimir_matriz(const char * file_name, double ** matriz, int n, int m) {
	std::cout<<"Datos de la matriz: "<<file_name<<std::endl;
	for (int current_sample = 0; current_sample < n; current_sample++) {
		for (int current_trafo = 0; current_trafo < m; current_trafo++) {
			printf("%.6f \t", matriz[current_sample][current_trafo]);
		}
		printf("\n");
	}
	printf("\n");
}

double ** get_matrix_from_file(const char * file_name, bool include_sums) {
	std::string medida;
	int current_trafo = 0;
	int current_sample = 0;

	//Apertura del archivo
	std::ifstream myfile;
	myfile.open(file_name, std::ios::in);
	//Cantidad de transformadores
	getline(myfile, medida, '\n');
	int limite = atoi(medida.c_str());
	//Cantidad de muestras
	getline(myfile, medida, '\n');
	int muestras = atoi(medida.c_str());

	int trafos = limite;
	if (include_sums) {
		trafos++;//una columna más para las sumas
	}
	double ** data = new double*[muestras];

	for (current_sample = 0; current_sample < muestras; current_sample++) {
		data[current_sample] = new double[trafos];
	}
	double sum = 0.0;
	if (myfile.is_open()) {
		for (current_sample = 0; current_sample < muestras; current_sample++) {
			char * c_medida;
			getline(myfile, medida);
			c_medida = (char*) medida.c_str();
			char * ptr = strtok(c_medida, ";");
			sum = 0.0;
			for (current_trafo = 0; current_trafo < limite; current_trafo++) {
				//guardar en una posicion de la matriz
				data[current_sample][current_trafo] = (double)atof(ptr);
				//std::cout << std::setprecision(16) <<data[current_sample][current_trafo]<<"\t";
				ptr = strtok(0, ";");
				sum += data[current_sample][current_trafo];
			}

			if (include_sums) {
				data[current_sample][limite] = sum;
			}
			//std::cout<<"\n";

		}
		//imprimir_matriz(file_name, data,muestras,limite);
		myfile.close();

	}
	return data;
}
std::string get_current_time(){
	std::stringstream timestream;
	time_t t = time(0);
	struct tm * now = localtime(&t);
	timestream << now->tm_mday << '-' << (now->tm_mon + 1) << '-'
			<< (now->tm_year + 1900) << '_' << now->tm_hour << ':'
			<< now->tm_min << ':' << now->tm_sec;
	return timestream.str();
}
