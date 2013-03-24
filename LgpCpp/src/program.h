/*
 * program.h
 *
 *  Created on: Mar 16, 2013
 *      Author: Vanessa Cañete, Nahuel Hernández
 */

#include "parameters.h"

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
	Instruction *list_inst;
	int height = 0;
	double *list_reg;
	//todas las mierdas para la ejecución del programa virtual_machine de tincho
};

Program::Program() {
	height = randint(NUM_MIN_INSTRUCTIONS, NUM_INI_INSTRUCTIONS);
	list_inst = new Instruction[height];

	//for (int i = 0; i < height; i ++) {

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
}

