/*
 * individual.h
 *
 *  Created on: Mar 16, 2013
 *      Author: Vanessa Cañete, Nahuel Hernández
 */



class Individual {
public:
	Individual();
	~Individual();
	Individual(const Individual& source);
	Individual& operator=(const Individual& source);
	void create_new_individual(int _config_position);

	//Evaluacion de fitness
	void eval_fitness();
	void set_altered();
	double evaluate(int obj);

	//operadores genéticos
	void check_max_min_instructions (std::string name, std::string place);
	static void crossover(Individual * &genome1, Individual * &genome2);
	//void exchange(Individual &mom, Individual &dad, int * cuts_points_mom, int * cuts_points_dad);
	void clone(Individual * orig);
	void print_individual();
	void exchange(const Individual * dest, const Individual * origin, int * cp_dest, int * cp_origin);

	//ordenan de mejor a peor
	static bool compare_fitness(Individual &x, Individual &y);
	static bool compare_error_prom(Individual &x, Individual &y);
	static bool compare_sigma(Individual &x, Individual &y);
	static bool compare_validation_error(Individual &x, Individual &y);

	void macro_mutation ();
	void micro_mutation ();
	std::string select_micro_mutacion_type(float prob);
	int get_random_register(int &op, std::vector<int> reg_eff, Instruction * instruction);

	Program * program;
	double fitness;
	double error;
	double validation_error;
	double sigma; //era dev
	int config_position;
	bool evaluated;
};

//para la función de comparación de std::sort
struct compare_ob1 {
	inline bool operator() (const Individual &x, const Individual &y) {
		if (x.error > y.error) {
			return false;
		} else if (x.error < y.error) {
			return true;
		} else if (x.error == y.error) {
			if (x.program->n_eff < y.program->n_eff) {
				return true;
			} else if (x.program->n_eff > y.program->n_eff) {
				return true;
			} else {
				return false;
			}
		}
	}
};


struct compare_ob2 {
	inline bool operator() (const Individual &x, const Individual &y) {
		if (x.sigma > y.sigma) {
			return false;
		} else if (x.sigma < y.sigma) {
			return true;
		} else if (x.sigma == y.sigma) {
			if (x.program->n_eff < y.program->n_eff) {
				return true;
			} else if (x.program->n_eff > y.program->n_eff) {
				return true;
			} else {
				return false;
			}
		}
	}
};



Individual::Individual() {
	fitness = 0.0;
	error = 0.0;
	sigma = 0.0;
	validation_error = -1;
	config_position = -1;
	program = 0;
	evaluated = false;
}

void Individual::create_new_individual(int _config_position) {
	config_position = _config_position;
	program = new Program;
	//program->create_new_program();
}

Individual::~Individual() {
	delete program;
}


Individual::Individual(const Individual& source) :
	fitness(source.fitness),
	error(source.error),
	sigma(source.sigma),
	validation_error(source.validation_error),
	config_position(source.config_position),
	evaluated(source.evaluated) {
	program = new Program(*source.program);
}

Individual& Individual::operator=(const Individual& source) {
	// check for self-assignment
	if (this == &source)
		return *this;

	fitness = source.fitness;
	error = source.error;
	sigma = source.sigma;
	validation_error = source.validation_error;
	config_position = source.config_position;
	evaluated = source.evaluated;

	delete program;
	program = new Program(*source.program);

	return *this;
}

void Individual::exchange(const Individual * dest, const Individual * origin, int * cp_dest, int * cp_origin) {
	config_position = dest->config_position;
	evaluated = false;
	int segment_out = cp_dest[1] - cp_dest[0];
	int segment_in = cp_origin[1] - cp_origin[0];

	int old_height = dest->program->height;
	int new_height = (old_height - segment_out) + segment_in;

	if (program) {
		delete program;
	}

	program = new Program(new_height);
	std::copy(dest->program->list_reg, dest->program->list_reg + NUM_INDIVIDUAL_REGISTERS, program->list_reg);


	Instruction * cp_dest_inst = dest->program->list_inst + cp_dest[0];
	Instruction * cp_orig_inst = origin->program->list_inst + cp_origin[0];
	Instruction * points [3];

	points[0] = program->list_inst;
	points[1] = points[0] + cp_dest[0];
	points[2] = points[1] + segment_in;

	std::copy(dest->program->list_inst, cp_dest_inst, points[0]);
	std::copy(cp_orig_inst, cp_orig_inst + segment_in, points[1]);
	std::copy(dest->program->list_inst + cp_dest[1], dest->program->list_inst + old_height, points[2]);

}

inline void Individual::set_altered() {
	fitness = 0.0;
	evaluated = false;
}

void Individual::eval_fitness() {
	if (evaluated) {
		//ya está evaluado
		return;
	}

	double error_quad[TRAINING_LINES];
	double error_a_quad = 0.0;
	double error_prom_quad = 0.0;
	double error_dev = 0.0;

	program->get_effective_instructions();
	//std::cout << "Errores en entrenamiento: \n";

	for (int t = 0; t < TRAINING_LINES; t++) {
		double * int_t = Program::R_CONST[t];
		double result = program->execute_program(int_t);
		error_quad[t] = pow((result - Program::DATA[t][config_position]), 2.0);
		//std::cout << "result: " << result << "   data: " << Program::DATA[t][config_position] << "\n",
		//std::cout << "error: " << error_quad[t] << "\n";
		//std::cout << "********************************************************" << "\n\n";
		//std::cout << "********************************************************" << "\n\n";
		error_a_quad += error_quad[t];
	}

	error_prom_quad = error_a_quad / TRAINING_LINES;
	for (int i = 0; i < TRAINING_LINES; i++) {
		error_dev += pow((error_prom_quad - error_quad[i]), 2);

	}
	error_dev /= (TRAINING_LINES- 1);

	if (error_prom_quad == 0.0) {
		error_prom_quad = 0.000000001;
	}
	error = error_prom_quad;
	sigma = sqrt(error_dev);

	fitness = 1 / ((W_OB1 * error) + (W_OB2 * sigma));

	evaluated = true;
}


inline double Individual::evaluate(int obj=FITNESS) {
	if (!evaluated) {
		std::cout << "EVALUAR FITNESS ANTES DE USAR ESTA FUNCION!!!!";
	}

	if (obj == FITNESS) {
		return fitness;
	} else if (obj == OB1) {
		return error;
	} else if (obj == OB2) {
		return sigma;
	}
}


inline bool Individual::compare_fitness(Individual &x, Individual &y) {
	if (x.evaluate() > y.evaluate()) {
		return false;
	} else if (x.evaluate() < y.evaluate()) {
		return true;
	} else if (x.evaluate() == y.evaluate()) {
		if (x.program->n_eff < y.program->n_eff) {
			return true;
		} else if (x.program->n_eff > y.program->n_eff) {
			return true;
		} else {
			return false;
		}
	}
}

inline bool Individual::compare_error_prom(Individual &x, Individual &y) {
	if (x.evaluate() > y.evaluate()) {
		return false;
	} else if (x.evaluate(OB1) < y.evaluate(OB1)) {
		return true;
	} else if (x.evaluate(OB1) == y.evaluate(OB1)) {
		if (x.program->n_eff < y.program->n_eff) {
			return true;
		} else if (x.program->n_eff > y.program->n_eff) {
			return true;
		} else {
			return false;
		}
	}
}


inline bool Individual::compare_sigma(Individual &x, Individual &y) {
	if (x.evaluate() > y.evaluate()) {
		return false;
	} else if (x.evaluate(OB2) < y.evaluate(OB2)) {
		return true;
	} else if (x.evaluate(OB2) == y.evaluate(OB2)) {
		if (x.program->n_eff < y.program->n_eff) {
			return true;
		} else if (x.program->n_eff > y.program->n_eff) {
			return true;
		} else {
			return false;
		}
	}
}


//devuelve true si x es mejor que y, es decir, x tiene menor error en validacion.
inline bool Individual::compare_validation_error(Individual &x, Individual &y) {
	if (x.validation_error > y.validation_error) {
		return false;
	} else if (x.validation_error < y.validation_error) {
		return true;
	} else if (x.validation_error == y.validation_error) {
		return false;
	}
}




void Individual::print_individual() {
	std::cout << "Config Pos: " << config_position << "\n";
	std::cout << "Training error: " << error << "\n";
	std::cout << "Trainig Deviation: " << sigma << "\n";
	std::cout << "Fitness: " << fitness << "\n";
	std::cout << "List Size: " << program->height << "\n";
	std::cout << "List effective Size: " << program->n_eff << "\n";

	std::cout <<  "\n";
	std::cout << "List Instruction: " << "\n";
	for (int i = 0; i < program->height; i++) {
		program->list_inst[i].print_instruction();
	}

	std::cout <<  "\n";
	std::cout << "List Effective: " << "\n";
	for (int i = 0; i < program->n_eff; i++) {
		//std::cout << i << "- ";
		program->effective_list_inst[i].print_instruction();
	}

	std::cout <<  "\n";
	//std::cout.precision(15);
	std::cout << "Registers: " << "\n";
	for (int i = 0; i < NUM_INDIVIDUAL_REGISTERS; i++) {
		std::cout <<  "r_all[" << i << "] = " << program->list_reg[i] << "\n";
	}

}

void Individual::clone(Individual * orig){
	fitness = orig->fitness;
	error = orig->error;
	sigma = orig->sigma;
	//index = orig->index;
	config_position = orig->config_position; //ver si hace falta
	evaluated = 0;
	program = new Program;
	*program = *orig->program;
	program->list_inst = 0;
	program->list_inst = new Instruction [orig->program->height];
	program->list_reg = new double [NUM_INDIVIDUAL_REGISTERS];
	std::copy(orig->program->list_inst, orig->program->list_inst + orig->program->height, program->list_inst );
	std::copy(orig->program->list_reg, orig->program->list_reg + NUM_INDIVIDUAL_REGISTERS, program->list_reg);
}


void Individual::check_max_min_instructions (std::string name, std::string lugar) {
	if (program->height> NUM_MAX_INSTRUCTIONS) {
       std::cout<<"[Individual::check_max_min_instructions]"<<name +
    		   " - Superó el número maximo de instrucciones ->" + lugar;
	}
    if (program->height < NUM_MIN_INSTRUCTIONS) {
       std::cout<<"[Individual::check_max_min_instructions]"<< name +
    		   " - Superó el número minimo de instrucciones ->" + lugar;
    }
}
/*
void Individual::exchange(Individual& g1, Individual& g2, int * g1_cuts_p, int * g2_cuts_p){
	/* Se remplaza el bloque 2 por el bloque 4.
	 *
	 * |----------|***|--------|  <-     |---|*****|----------------| = |----------|*****|--------|
	 * |    1     | 2 |    3   |  <-     | 3 |  4  |        5       |   |    1     |  4  |    3   |
	 */
	//se calcula la nueva longitud = len(1) + len(4) + len(3)
	/*
	int new_len = g1_cuts_p[0];
	new_len += (g2_cuts_p[1] - g2_cuts_p[0]) + 1;
	new_len += (g1.program->height - g1_cuts_p[1]) - 1;
	//se borra la lista actual
	delete [] program->list_inst;
	//Se crea una nueva lista
	program->list_inst = new Instruction[new_len];
	program->height = new_len;
	//Se copia parte 1

	/*
	int pos = 0;
	//se copia parte 1
	for (i = 0; i < g1_cuts_p[0]; i++){
		program->list_inst[pos] = g1.program->list_inst[i];
		pos++;
	}
	//Se copia la parte 4 - incluye ambos limites
	for (i = g2_cuts_p[0]; i <= g2_cuts_p[1] ; i++){
		program->list_inst[pos] = g2.program->list_inst[i];
		pos++;
	}
	//Se copia la parte 2 - se corre uno ya que se remplazo el punto de corte
	for (i = g1_cuts_p[1]+1; i < g1.program->height ; i++){
		program->list_inst[pos] = g1.program->list_inst[i];
		pos++;
	}
	set_altered();
}

*/

void Individual::crossover(Individual * &genome1, Individual * &genome2) {//, Individual * &child1, Individual * &child2
	genome1->check_max_min_instructions("genome1", "Antes Crossover");
	genome2->check_max_min_instructions("genome2", "Antes Crossover");
	Individual * mom, * sister;
	Individual * dad, * brother;

	int cuts_points_mom[2];
	int cuts_points_dad[2];
	int max_segment_size_mom;
	int mom_segment_size;
	int max_padding_mom;
	int max_segment_full_mom;
	int max_segment_num_min_in_dad;
	int max_segment_size_dad;
	int min_segment_full_dad;
	int min_segment_size_num_min_in_mom;
	int min_segment_size_dad;
	int dad_segment_size;
	int max_padding_dad;
	bool change = false;

	if (genome1->program->height >= genome2->program->height) {
		mom = genome2;
		dad = genome1;
		change = true;
		//sister = dad;
		//brother = child1;
	} else {
		mom = genome1;
		dad = genome2;
		//sister = child1;
		//brother = child2;
	}

	try {
		max_segment_size_mom = mom->program->height - 1;
		//al menos 1 instruccion menos la ultima.
		mom_segment_size = randint(1, max_segment_size_mom);
		max_padding_mom = mom->program->height - mom_segment_size - 1;
		//desde donde se puede comenzar para que alcancen las instrucciones del segmento a cruzar
		cuts_points_mom[0] = randint(0, max_padding_mom);
		cuts_points_mom[1] = cuts_points_mom[0] + mom_segment_size;

		//esto ya se aseguró en el padding
		/*para que no incluya a la ultima instruccion.
		cuts_points_mom[1] =
			max_padding_mom	(cuts_points_mom[1] == mom.program->height - 1) ?
						cuts_points_mom[1] - 1 : cuts_points_mom[1];
		*/

		//el máximo segmento a cruzar es lo que le falta al mom para completar el máximo número de instrucciones permitidas
		max_segment_full_mom = NUM_MAX_INSTRUCTIONS - (mom->program->height - mom_segment_size);
		max_segment_num_min_in_dad = (dad->program->height + mom_segment_size) - NUM_MIN_INSTRUCTIONS;
		//se elije el menor de los máximos

		if (max_segment_full_mom < max_segment_num_min_in_dad) {
			max_segment_size_dad = max_segment_full_mom;
		} else {
			max_segment_size_dad = max_segment_num_min_in_dad;
		}

		//si el maximo es mayor a la longitud del padre, se elige la longitud como máximo
		if ((dad->program->height - 1) < max_segment_size_dad) {
			max_segment_size_dad = (dad->program->height - 1);
		}

		//lo que hay que quitarle para que quede en máximo número de instrucciones
		min_segment_full_dad = (dad->program->height + mom_segment_size) - NUM_MAX_INSTRUCTIONS;

		//lo que falta para completar el mínimo número de instrucciones al mom
		min_segment_size_num_min_in_mom = NUM_MIN_INSTRUCTIONS - (mom->program->height - mom_segment_size);

		if (min_segment_full_dad > min_segment_size_num_min_in_mom) {
			min_segment_size_dad = min_segment_full_dad;
		} else {
			min_segment_size_dad = min_segment_size_num_min_in_mom;
		}

		if (min_segment_size_dad <= 0) {
			min_segment_size_dad = 1;
		}


		dad_segment_size = randint(min_segment_size_dad, max_segment_size_dad);
		max_padding_dad = dad->program->height - dad_segment_size - 1;

		//desde donde se puede comenzar para que alcancen las instrucciones del segmento a cruzar

		cuts_points_dad[0] = randint(0, max_padding_dad);
		cuts_points_dad[1] = cuts_points_dad[0] + dad_segment_size;

		//Se crean nuevos temporales
		sister = new Individual;
		brother = new Individual;

		std::cout << "cp mon : " << cuts_points_mom[0] << " , " << cuts_points_mom[1] << "\n";
		std::cout << "cp dad : " << cuts_points_dad[0] << " , " << cuts_points_dad[1] << "\n";

		//Se intercambian los bloques en los nuevos creados
		sister->exchange(mom, dad, cuts_points_mom, cuts_points_dad);
		brother->exchange(dad, mom, cuts_points_dad, cuts_points_mom);

		//se borran las copias creadas para la macro y micro
		delete genome1;
		delete genome2;

		if (change) {
			genome2 = sister;
			genome1 = brother;
		} else {
			genome1 = sister;
			genome2 = brother;
		}

		sister->check_max_min_instructions("sister", "Despues Crossover");
		brother->check_max_min_instructions("brother", "Despues Crossover");

	} catch (std::exception e) {
		std::cout << "Error en Crossover";
		std::cout << e.what();

	}
}

void Individual::macro_mutation() {
	//Agrega o quita instrucciones  -- Alg. 6.1 -- p_ins > p_del //
	int insertion = random_flip_coin(P_INS);
	int mutation_point = randint(0, program->height - 2);
	if (program->height < NUM_MAX_INSTRUCTIONS
			&& (insertion || program->height == NUM_MIN_INSTRUCTIONS)) {
		// Si no supera la max. cant. de instrucciones y es insercion o tiene el numero minimo de instrucciones
		Instruction new_instruction;
		std::vector<int> reg_eff;
		int to_mutate = program->get_effective_registers(mutation_point,
				reg_eff);
		while (reg_eff.empty()) {
			/* se da en el caso de que el punto de mutación esté por debajo de la última
			 * instrucción efectiva y ésta sea unaria con un operando constante
			 * cambiar el registro constante del operador unario por uno variable
			 */
			program->list_inst[to_mutate].op2 = randint(VAR_MIN, VAR_MAX);
			to_mutate = program->get_effective_registers(mutation_point,
					reg_eff);
		}

		new_instruction.oper = reg_eff.at(randint(0, reg_eff.size() - 1));
		Instruction * new_list = new Instruction[program->height + 1];

		/*
		 * Insertar la nueva instrucciòn en la posición mutation_point
		 */
		int pos = 0;
		int i = 0;
		//se copia parte 1
		for (i = 0; i < mutation_point; i++) {
			new_list[pos] = program->list_inst[i];
			pos++;
		}

		//Se copia la parte 4 - incluye ambos limites
		new_list[pos] = new_instruction;
		pos++;
		//Se copia la parte 2 - se corre uno ya que se remplazo el punto de corte
		for (i = mutation_point; i < program->height; i++) {
			new_list[pos] = program->list_inst[i];
			pos++;
		}
		set_altered();
		program->height += 1;
		program->list_inst = new_list;
	}
	if (program->height > NUM_MIN_INSTRUCTIONS
			&& (!insertion || program->height == NUM_MAX_INSTRUCTIONS)) {
		//Si es mayor a la  min. cant. de instrucciones y  no es insercion o tiene el numero maximo de instrucciones
		int i = 0;
		for (i = mutation_point; i < program->height - 1; i++) {
			program->list_inst[i] = program->list_inst[i + 1];
		}
		program->height -= 1;
		set_altered();
	}
	if (program->height > NUM_MAX_INSTRUCTIONS) {
		std::cout << "superado el número maximo de instrucciones MACRO";
		std::cout << "genome.height > Parameters.num_max_instructions";
	}
}

int Individual::get_random_register(int &pos_to_replace,
		std::vector<int> reg_eff = { }, Instruction * instruction = 0) {
	int _register = 0;
	if (pos_to_replace == 1) { //destino
		if (reg_eff.empty()) {
			if (instruction->oper < 5) {
				//se hace que se mute uno de los registros operandos
				pos_to_replace = random_flip_coin(0.5) ? 2 : 3;
			} else {
				//#operación unaria, se muta el operando 2 para que sea efectiva
				pos_to_replace = 3;
			}
		} else {
			int n = randint(0, reg_eff.size() - 1);
			_register = reg_eff.at(n);
		}
	}
	if (pos_to_replace == 2) { //operando 1
		//Cambio: se permite al primer operando ser variable o constante de entrada
		if (random_flip_coin(0.5))
			_register = randint(VAR_MIN, VAR_MAX);
		else
			_register = randint(CONST_IN_MIN, CONST_IN_MAX);
		_register = randint(VAR_MIN, VAR_MAX);
	}
	if (pos_to_replace == 3) {        //operando 2
		//operador 2 es constante con probabilidad p_const
		if (random_flip_coin (P_REG_OP2_CONST)) {
			//Si va a ser constante, las constantes de entrada tienen mayor
			//probabilidad que las aleatorias.
			if (random_flip_coin (P_CONST_IN))
				_register = randint(CONST_IN_MIN, CONST_IN_MAX);
			else
				_register = randint(CONST_AL_MIN, CONST_AL_MAX);
		} else {
			_register = randint(VAR_MIN, VAR_MAX);
		}
	}
	return _register;
}

void Individual::micro_mutation() {
	/* Muta las instrucciones efectivas internamente
	 * p_regmut = probabilidad de mutar un registro
	 * p_opermut = probabilidad de mutar una operación
	 * p_constmut = probabilidad de mutar una constante efectiva
	 */
	program->get_effective_instructions();
	int * indices = program->get_effective_instructions_with_indices();
	std::vector<int> v_indices;
	int index = randint(0, program->n_eff - 1);
	int mutation_point = indices[index];
	int pos_to_replace = 0;
	std::string type = select_micro_mutacion_type(_random());
	int op = -1;
	if (type == "constantes") {
		std::vector<int> constants_indices =
				program->get_effective_constant_indices();
		if (!constants_indices.empty()) {
			int ins_with_constant_index = constants_indices.at(
					randint(0, constants_indices.size() - 1));
			int register_mutation_index =
					program->list_inst[ins_with_constant_index].op2;
			program->list_reg[register_mutation_index] += pow((-1),
					(randint(0, 1)) * randfloat(0.0, STEP_SIZE_CONST));
		} else {
			//no hay instrucciones efectivas con registros constantes variables
			type = select_micro_mutacion_type(_random());
			//si vuelve a salir constante, se elige mutación de registro o operaciones con igual probabilidad
			if (type == "constantes") {
				type = random_flip_coin(0.5) ? "registros" : "operaciones";
			}
		}
	}

	if (type == "registros") {
		if (program->n_eff == 1) {
			//una sola instrucción efectiva, no se puede cambiar r[0]
			if (program->list_inst[mutation_point].oper < 5) {
				pos_to_replace = randint(2, 3);
			} else { //operación unaria, cambiar el segundo operando
				pos_to_replace = 3;
			}
		} else {
			if (program->list_inst[mutation_point].oper < 5)
				pos_to_replace = randint(1, 3);
			else
				//operación unaria, cambiar el segundo operando o el destino
				pos_to_replace = random_flip_coin(0.5) ? 1 : 3;
		}
		if (pos_to_replace == 1) { //destino
			//si no es el último índice, la última

			if ((index + 1) < program->n_eff) {
				/*Se obtiene la lista de registros efectivos en esa posición, para reemplazar
				 * en la instrucción y permitir que siga siendo efectiva                 *
				 */
				std::vector<int> reg_eff;
				int to_mutate = program->get_effective_registers(
						indices[index + 1], reg_eff);
				if (!reg_eff.empty()) {
					std::vector<int>::iterator i = std::find(reg_eff.begin(),
							reg_eff.end(),
							program->list_inst[mutation_point].dest);
					if (i != reg_eff.end()) {
						reg_eff.erase(i); //remover el registro destino de los registros efectivos
						op = get_random_register(pos_to_replace, reg_eff,
								&program->list_inst[mutation_point]);
					}
				}
			} else { //el punto de mutación es la última instrucción con el r[0]
				if (program->list_inst[mutation_point].oper < 5)
					pos_to_replace = randint(2, 3);
				else
					//operación unaria, cambiar el segundo operando
					pos_to_replace = 3;
				op = get_random_register(pos_to_replace);
			}
		} else { //para los casos de operandos op1 y op2
			op = get_random_register(pos_to_replace);
		}
		if (pos_to_replace == 1)
			program->list_inst[mutation_point].dest = op;
		if (pos_to_replace == 2)
			program->list_inst[mutation_point].op1 = op;
		if (pos_to_replace == 3)
			program->list_inst[mutation_point].op2 = op;
	}
	int diff_op = 0;
	if (type == "operaciones") {
		diff_op = randint(OP_MIN, OP_MAX);
		while (program->list_inst[mutation_point].oper == diff_op)
			diff_op = randint(OP_MIN, OP_MAX);
		program->list_inst[mutation_point].oper = diff_op;
	}
	set_altered();
	//para borrar desde aca
	std::cout<<"Mutation type = "<<type<<"\nMutation_point = "<<mutation_point<< "\npos_to_replace = "<<pos_to_replace<<"\n";
	if (type=="operaciones"){
		std::cout<<"Nueva operacion = " << diff_op <<"\n";
	}else{
		std::cout<<"Nuevo Registro = " << op<<"\n";
	}
	//hasta aca
}

std::string Individual::select_micro_mutacion_type(float prob) {
	if (prob <= P_REGMUT) {
		return "registros";
	} else {
		if (prob > P_REGMUT && prob <= P_OPERMUT + P_REGMUT) {
			return "operaciones";
		} else if (prob > (P_OPERMUT + P_REGMUT)
				&& prob <= (P_OPERMUT + P_REGMUT + P_CONSTMUT)) {
			return "constantes";
		}
	}
	return "";
}

