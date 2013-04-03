/*
 * program.h
 *
 *  Created on: Mar 16, 2013
 *      Author: Vanessa Cañete, Nahuel Hernández
 */


class Instruction {
public:
	Instruction (); //create_new_instruction();
	int oper;
	int dest;
	int op1;
	int op2;
};


Instruction::Instruction() {
	/*
	reg_sal                      -> registro de salida
	op_min, op_max               -> instrucciones
	CONST_in_min, CONST_in_max     -> registros de entrada constantes --> estan en r_const
	CONST_al_min, CONST_al_max     -> Constantes aleatorias inicializadas entre (0, random_max)
	var_min, var_max             -> registros variables inicializados a 1.
	*/

	oper = randint(OP_MIN, OP_MAX); //Instrucciones
	dest = randint(VAR_MIN, VAR_MAX);  //Registros destinos - Solo los variables
	op1 = randint(VAR_MIN, VAR_MAX); //Solo puede ser variable.

	//operador 1 es constante de entrada con probabilidad P_REG_OP1_CONST
	if (random_flip_coin(P_REG_OP1_CONST)) {
		op1 = randint(CONST_IN_MIN, CONST_IN_MAX);
	} else {
		op1 = randint(VAR_MIN, VAR_MAX);
	}

	//operador 2 es constante con probabilidad p_const
	if (random_flip_coin(P_REG_OP2_CONST)) {
		//Si va a ser constante, las constantes de entrada tienen mayor probabilidad que las aleatorias.
		if (random_flip_coin(P_CONST_IN)) {
			op2 = randint(CONST_IN_MIN, CONST_IN_MAX);
		} else {
			op2 = randint(CONST_AL_MIN, CONST_AL_MAX);
		}
	} else {
		op2 = randint(VAR_MIN, VAR_MAX);
	}
}




class Program {
public:
	Program (); // sería ini_individual()
	~Program();
	Instruction* get_effective_instructions();
	double execute_program(double * input);
	int* get_effective_instructions_with_indices();
	std::vector<int> get_effective_constant_indices();
	std::vector<int> get_effective_registers(int * position);
	static void init_registers();

	Instruction *list_inst;
	Instruction *effective_list_inst; //puntero dentro de la lista de abajo
	Instruction *effective_memory_space;
	int *effective_indices;
	int height;
	int height_eff_space;
	int n_eff;
	double *list_reg;

	static double R_OUT[NUM_OUT_REGISTERS];
	static double R_VAR[NUM_VAR_REGISTER];
	static double R_MATH_CONST[NUM_CONST_MATH_REGISTERS];
	static double ** R_CONST;//[LINES][K];
	static double ** DATA;

};

double Program::R_OUT[NUM_OUT_REGISTERS] = {};
double Program::R_VAR[NUM_VAR_REGISTER] = {};
double Program::R_MATH_CONST[NUM_CONST_MATH_REGISTERS] = {};
double ** Program::R_CONST = new double*[LINES];
double ** Program::DATA = 0;

void Program::init_registers() {
	R_OUT[0] = INIT_VAR_VALUE;

	for (int i = 0; i < NUM_VAR_REGISTER; i++) {
		R_VAR[i] = INIT_VAR_VALUE;
	}

	R_MATH_CONST[0] = 0.0;
	R_MATH_CONST[1] = 1.0;
	R_MATH_CONST[2] = M_E;
	R_MATH_CONST[3] = M_PI;


	//se carga la matriz desde el archivo
	DATA = get_matrix_from_file();
	//se imprime la matriz
	//imprimir_matriz(DATA, LINES, N);

	for (int t = 0; t < LINES; t++) {
		R_CONST[t] = new double[K];
	}

	for (int t = 0; t < LINES; t++) {
		for (int i = 0, j = 0; i < N; i++) {
			//std::cout << "t: " << t << " i: " <<  i <<"\n";
			if (CONFIG[i] == '1') {
				R_CONST[t][j] = DATA[t][i];
				j++;
			}
		}
	}
	imprimir_matriz(R_CONST, LINES, K);
}

Program::Program() {
	height = randint(NUM_MIN_INSTRUCTIONS, NUM_INI_INSTRUCTIONS);
	n_eff = -1;
	list_inst = new Instruction[height];
	effective_list_inst = 0;
	effective_memory_space = 0;
	effective_indices = 0;
	height_eff_space = 0;

	//Asegurar que la última instrucción tenga como registro destino al registro de salida
	list_inst[height - 1].dest = REG_OUT;

	list_reg = new double[NUM_INDIVIDUAL_REGISTERS];

	int offset = 0;
	for (int i = 0; i < NUM_OUT_REGISTERS; i++) {
		list_reg[offset + i] = R_OUT[i];
	}
	offset += NUM_OUT_REGISTERS;

	for (int i = 0; i < NUM_VAR_REGISTER; i++) {
		list_reg[offset + i] = R_VAR[i];
	}
	offset += NUM_VAR_REGISTER;

	for (int i = 0; i < NUM_CONST_RANDOM_REGISTERS; i++) {
		list_reg[offset + i] = randfloat(CONST_MIN, CONST_MAX);
	}
	offset += NUM_CONST_RANDOM_REGISTERS;

	for (int i = 0; i < NUM_CONST_MATH_REGISTERS; i++) {
		list_reg[offset + i] = R_MATH_CONST[i];
	}
}

Program::~Program() {
	delete [] list_inst;
	delete [] list_reg;
	delete [] effective_memory_space;
	delete [] effective_indices;
}


Instruction* Program::get_effective_instructions() {
	std::set<int> reg_eff;
	std::set<int>::iterator it;

	int indice = height;
	int indice_indice = 0;

	if (height_eff_space < height) {
		if (effective_memory_space) {
			delete [] effective_memory_space;
			delete [] effective_indices;
		}
		effective_memory_space = new Instruction[height];
		effective_indices = new int[height];
		height_eff_space = height;
	}

	//std::vector<Instruction> eff_i;

	reg_eff.insert(0);
	for (int i = height - 1; i >= 0 ; i--) {
		it = reg_eff.find(list_inst[i].dest);
		if (it != reg_eff.end()) {
			reg_eff.erase(it);

			if (list_inst[i].oper < 5) { //los operadores unarios tiene identificador del 5 al 9
				if (list_inst[i].op1 <= VAR_MAX) { //los registros constantes de entrada no pueden ser registros efectivos
					reg_eff.insert(list_inst[i].op1);
				}
			}

			if (list_inst[i].op2 <= VAR_MAX) {
				reg_eff.insert(list_inst[i].op2);
			}

			//eff_i.push_back(list_inst[i]);
			indice--;
			effective_memory_space[indice] = list_inst[i];
			effective_indices[indice_indice] = i;
			indice_indice++;
		}
	}

	//Se setea un atributo con el número de instrucciones efectivas del individuo
	//self.n_eff = len(eff_i)
	//eff_i.reverse()
	n_eff = height - indice;
	effective_list_inst = &(effective_memory_space[indice]);

	return effective_list_inst;
}

int* Program::get_effective_instructions_with_indices() {
	return effective_indices;
}

std::vector<int> Program::get_effective_constant_indices() {
	std::vector<int> indices;

	for (int i = 0; i < n_eff; i++) {
		if (effective_list_inst[i].op2 > CONST_AL_MIN && effective_list_inst[i].op2 < CONST_AL_MAX) {
			indices.push_back(effective_indices[n_eff - i]);
		}
	}

	return indices;

}

std::vector<int> Program::get_effective_registers(int * position) {
	std::vector<int> indices;
	std::set<int> reg_eff;
	std::set<int>::iterator it;


	reg_eff.insert(0);
	for (int i = n_eff - 1; i >= 0 ; i--) {
		it = reg_eff.find(effective_list_inst[i].dest);
		if (it != reg_eff.end()) {
			reg_eff.erase(it);

			if (effective_list_inst[i].oper < 5) { //los operadores unarios tiene identificador del 5 al 9
				if (effective_list_inst[i].op1 <= VAR_MAX) { //los registros constantes de entrada no pueden ser registros efectivos
					reg_eff.insert(effective_list_inst[i].op1);
				}
			}

			if (effective_list_inst[i].op2 <= VAR_MAX) {
				reg_eff.insert(effective_list_inst[i].op2);
			}
		}
		if (effective_indices[i] == *position || reg_eff.empty()) {
			std::copy(reg_eff.begin(), reg_eff.end(), std::back_inserter(indices));
			//se pasa por referencia se copia en position el valor de la posición, en python se devolvían dos cosas
			*position = effective_indices[i];

			return indices; //return list(reg_eff), current_pos
		}
	}

	return indices;//al pedo
}


double Program::execute_program(double * input) {
	double r_all[NUM_INDIVIDUAL_REGISTERS];
	std::copy(list_reg, list_reg + height, r_all);
	Instruction *instructions;

	instructions = get_effective_instructions();

	for (int i = 0; i < n_eff; i++) {
		double operand_1, operand_2;

		operand_1 = input[instructions[i].op1 - REGISTER_OFFSET] ? instructions[i].op1 > REGISTER_OFFSET : r_all[instructions[i].op1];
		operand_2 = input[instructions[i].op2 - REGISTER_OFFSET] ? instructions[i].op2 > REGISTER_OFFSET : r_all[instructions[i].op2];


		switch (instructions[i].oper) {
		case ADD: {
			r_all[instructions[i].dest] = operand_1 + operand_2;
			break;
		}
		case SUB: {
			r_all[instructions[i].dest] = operand_1 - operand_2;
			break;
		}
		case MUL: {
			r_all[instructions[i].dest] = operand_1 * operand_2;
			break;
		}
		case DIV: {
			if (operand_2 == 0) {
				r_all[instructions[i].dest] = operand_1 + C_UNDEF;
			} else {
				r_all[instructions[i].dest] = operand_1 / operand_2;
			}
			break;
		}
		case POW2: {
			r_all[instructions[i].dest] = pow(operand_2, 2);
			break;
		}
		case LOG10: {
			if (operand_2 == 0) {
				r_all[instructions[i].dest] = operand_1 + C_UNDEF;
			} else {
				r_all[instructions[i].dest] = log10(abs(operand_2));
			}
			break;
		}
		case SIN: {
			r_all[instructions[i].dest] = sin(operand_2);
			break;
		}
		case COS: {
			r_all[instructions[i].dest] = cos(operand_2);
			break;
		}
		}
	}

	return r_all[0];

}


