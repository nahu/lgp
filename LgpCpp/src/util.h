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
void parameters_to_file(std::string f_param){
    std::ofstream f;
    f.open(f_param.c_str());
    f<<"PARÁMETROS\n";
    f<<"----------------------------------------------------------------\n";
    f<<"--Problema\n";
    f<<"config\t"<<CONFIG<<"\n";
    f<<"n\t"<<N<<"\n";
    f<<"k\t"<<K<<"\n";


    f<<"\n----Muestra de datos\n";
    f<<"lines\t"<<LINES<<"\n";
    f<<"training_lines\t"<<TRAINING_LINES<<"\n";
    f<<"validation_lines\t"<<VALIDATION_LINES<<"\n";


    f<<"\n--Algoritmo Evolutivo\n";
    f<<"num_generations\t"<<NUM_GENERATIONS<<"\n";
    f<<"pool_size\t"<<POOL_SIZE<<"\n";
    f<<"population_size\t"<<POPULATION_SIZE<<"\n";
    f<<"demes\t"<<DEMES<<"\n";
    f<<"migration_rate\t"<<MIGRATION_RATE<<"\n";
    f<<"gen_to_migrate\t"<<GEN_TO_MIGRATE<<"\n";
    f<<"w_ob1\t"<<W_OB1<<"\n";
    f<<"w_ob2\t"<<W_OB2<<"\n";

    f<<"\n--Probabilidades\n";
    f<<"p_migration\t"<<P_MIGRATION<<"\n";
    f<<"p_migration_criteria\t"<<P_MIGRATION_CRITERIA<<"\n";
    f<<"--Registros constantes\n";
    f<<"p_reg_op1_const\t"<<P_REG_OP1_CONST<<"\n";
    f<<"p_reg_op2_const\t"<<P_REG_OP2_CONST<<"\n";
    f<<"p_const_in\t"<<P_CONST_IN<<"\n";

    f<<"\n----Crossover\n";
    f<<"p_crossover\t"<<P_CROSSOVER<<"\n";

    f<<"\n----Mutación Macro\n";
    f<<"p_macro_mutation\t"<<P_MACRO_MUTATION<<"\n";
    f<<"p_del\t"<<P_DEL<<"\n";
    f<<"p_ins\t"<<P_INS<<"\n";

    f<<"\n----Mutación Micro\n";
    f<<"p_micro_mutation\t"<<P_MICRO_MUTATION<<"\n";
    f<<"p_opermut\t"<<P_OPERMUT<<"\n";
    f<<"p_regmut\t"<<P_REGMUT<<"\n";
    f<<"p_constmut\t"<<P_CONSTMUT<<"\n";

    f<<"\n--Registros\n";
    f<<"\n----Cantidades\n";
    f<<"num_registers\t"<<NUM_REGISTERS<<"\n";
    f<<"   num_out_registers\t"<<NUM_OUT_REGISTERS<<"\n";
    f<<"num_var_register\t"<<NUM_VAR_REGISTER<<"\n";
    f<<"num_conts_registers\t"<<NUM_CONTS_REGISTERS<<"\n";
    f<<"   num_const_random_registers\t"<<NUM_CONST_RANDOM_REGISTERS<<"\n";
    f<<"   num_const_in_registers\t"<<NUM_CONST_IN_REGISTERS<<"\n";

    f<<"\n--Valores en vector instrucción\n";
    f<<"\n----Operaciones\n";
    f<<"num_operators\t"<<NUM_OPERATORS<<"\n";
    f<<"op_min\t"<<OP_MIN<<"\n";
    f<<"op_max\t"<<OP_MAX<<"\n";

    f<<"\n----Salida\n";
    f<<"reg_out\t"<<REG_OUT<<"\n";

    f<<"\n----Variables\n";
    f<<"var_min\t"<<VAR_MIN<<"\n";
    f<<"var_max\t"<<VAR_MAX<<"\n";

    f<<"\n----Constantes aleatorias\n";
    f<<"cons_al_min\t"<<CONST_AL_MIN<<"\n";
    f<<"cons_al_max\t"<<CONST_AL_MAX<<"\n";

    f<<"\n------Valores\n";
    f<<"const_min\t"<<CONST_MIN<<"\n";
    f<<"const_max\t"<<CONST_MAX<<"\n";
    f<<"step_size_const\t"<<STEP_SIZE_CONST<<"\n";

    f<<"\n----Constantes de entrada\n";
    f<<"cons_in_min\t"<<CONST_IN_MIN<<"\n";
    f<<"cons_in_max\t"<<CONST_IN_MAX<<"\n";


    f<<"\n--Instrucciones\n";
    f<<"num_ini_instructions\t"<<NUM_INI_INSTRUCTIONS<<"\n";
    f<<"num_max_instructions\t"<<NUM_MAX_INSTRUCTIONS<<"\n";
    f<<"num_min_instructions\t"<<NUM_MIN_INSTRUCTIONS<<"\n";


    f<<"\n--Procesos\n";
    f<<"chunk_size\t"<< CHUNK_SIZE<< "\n";
    f<<"num_processors\t"<<NUM_PROCESSORS<<"\n";

    f.close();
}

