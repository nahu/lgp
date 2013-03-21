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
	cons_in_min, cons_in_max     -> registros de entrada constantes --> estan en r_const
	cons_al_min, cons_al_max     -> Constantes aleatorias inicializadas entre (0, random_max)
	var_min, var_max             -> registros variables inicializados a 1.
	*/

	instruction = []
	instruction.append(random.randint(Parameters.op_min, Parameters.op_max)) #Instrucciones
	instruction.append(random.randint(Parameters.var_min, Parameters.var_max))  #Registros destinos - Solo los variables
	instruction.append(random.randint(Parameters.var_min, Parameters.var_max)) #Solo puede ser variable.

	#operador 2 es constante con probabilidad p_const
	if Util.random_flip_coin(Parameters.p_reg_op2_const):
		#Si va a ser constante, las constantes de entrada tienen mayor probabilidad que las aleatorias.
		if Util.random_flip_coin(Parameters.p_const_in):
			instruction.append(random.randint(Parameters.cons_in_min, Parameters.cons_in_max))
		else:
			instruction.append(random.randint(Parameters.cons_al_min, Parameters.cons_al_max))
	else:
		instruction.append(random.randint(Parameters.var_min, Parameters.var_max))

	return instruction

}




class Program {
public:
	Program (); // sería ini_individual()
	Instruction *list_inst;
	double *list_reg;
	//todas las mierdas para la ejecución del programa virtual_machine de tincho
};


