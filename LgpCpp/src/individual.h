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
	void check(int deme, int index);

	//Evaluacion de fitness
	void eval_fitness();
	double * eval_individual(int tipo);
	double * eval_individual_result(int tipo);
	void set_altered();
	double evaluate(int obj) const;

	//operadores genéticos
	void check_max_min_instructions(std::string name, std::string place);
	static void crossover(Individual * &genome1, Individual * &genome2);
	void clone(Individual * orig);
	void print_individual(bool resume=false);
	void exchange(const Individual * dest, const Individual * origin, int * cp_dest, int * cp_origin);

	//ordenan de mejor a peor
	static bool compare_fitness(Individual &x, Individual &y);
	static bool compare_error_prom(Individual &x, Individual &y);
	static bool compare_sigma(Individual &x, Individual &y);
	static bool compare_validation_error(Individual &x, Individual &y);

	void macro_mutation();
	void micro_mutation();
	static int select_micro_mutacion_type(float prob);
	static int get_random_register(int &op, Instruction * instruction, std::vector<int> reg_eff);
	int manhattan_op_distance(const Individual * ind);
	void print_op_diversity();
	bool check_op_diversity();
	Program * program;

	double fitness;
	double error;
	double validation_error;
	double sigma; //era dev
	int config_position;
	bool evaluated;

	static std::vector<int> cant_macro;
	static std::vector<int> cant_macro_del;
	static std::vector<int> cant_macro_ins;
	static std::vector<int> cant_micro;
	static std::vector<int> cant_micro_reg;
	static std::vector<int> cant_micro_const;
	static std::vector<int> cant_micro_ope;
	static std::vector<int> cant_crossover;
};
std::vector<int> Individual::cant_macro(NUM_PROCESSORS,0);
std::vector<int> Individual::cant_macro_del(NUM_PROCESSORS,0);
std::vector<int> Individual::cant_macro_ins(NUM_PROCESSORS,0);
std::vector<int> Individual::cant_micro(NUM_PROCESSORS,0);
std::vector<int> Individual::cant_micro_reg(NUM_PROCESSORS,0);
std::vector<int> Individual::cant_micro_const(NUM_PROCESSORS,0);
std::vector<int> Individual::cant_micro_ope(NUM_PROCESSORS,0);
std::vector<int> Individual::cant_crossover(NUM_PROCESSORS,0);
/*
 * Inicializacion de variables (para que no de "undefined reference to"
 */


//para la función de comparación de std::sort
struct compare_ob1 {
	inline bool operator()(const Individual &x, const Individual &y) {
		//borrar despues
		if (!x.evaluated || !y.evaluated ) {
			std::cout << "compare_ob1 EVALUAR FITNESS ANTES DE USAR ESTA FUNCION!!!!\n";
			std::cout << "x fitness y n_eff \n";
			std::cout <<  x.fitness << ", ";
			std::cout << x.program->n_eff << "\n";

			std::cout << "y fitness y n_eff \n";
			std::cout <<  y.fitness << ", ";
			std::cout << y.program->n_eff << "\n";
		}
		//hasta aca

		if (x.error > y.error) {
			return false;
		} else if (x.error < y.error) {
			return true;
		} else if (x.error == y.error) {
			return false;
		}
	}
};



struct compare_ob2 {
	inline bool operator()(const Individual &x, const Individual &y) {
		//borrar despues
		if (!x.evaluated || !y.evaluated ) {
			std::cout << "compare_ob1 EVALUAR FITNESS ANTES DE USAR ESTA FUNCION!!!!\n";
			std::cout << "x fitness y n_eff \n";
			std::cout <<  x.fitness << ", ";
			std::cout << x.program->n_eff << "\n";

			std::cout << "y fitness y n_eff \n";
			std::cout <<  y.fitness << ", ";
			std::cout << y.program->n_eff << "\n";
		}
		//hasta aca
		if (x.sigma > y.sigma) {
			return false;
		} else if (x.sigma < y.sigma) {
			return true;
		} else if (x.sigma == y.sigma) {
			return false;
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
	if (program) {
		delete program;
	}
}


Individual::Individual(const Individual& source) :
		fitness(source.fitness),
		error(source.error),
		sigma(source.sigma),
		validation_error(source.validation_error),
		config_position(source.config_position),
		evaluated(source.evaluated) {

	if (source.program) {
		program = new Program(*source.program);
	} else {
		program = 0;
	}
}



Individual& Individual::operator=(const Individual& source) {
	//auto-asingación
	if (this == &source) {
		//std::cout << "AUTOASIGNACION\n";
		return *this;
	}

	fitness = source.fitness;
	error = source.error;
	sigma = source.sigma;
	validation_error = source.validation_error;
	config_position = source.config_position;
	evaluated = source.evaluated;

	if (program) {
		delete program;
	}

	if (source.program) {
		program = new Program(*source.program);
	} else {
		program = 0;
	}

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
	Instruction * points[3];

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
	double * int_t;
	double result;

	program->get_effective_instructions();
	//std::cout << "Errores en entrenamiento: \n";

	for (int t = 0; t < TRAINING_LINES; t++) {
		int_t = Program::R_CONST[t];

#ifdef Q
		int offset = Q;
#else
		int offset = K;
#endif

		/*
		for (int index = 1; index < DELTA; ++index) { // La posición offset está ocupada por el promedio de las mediciones
			if ((t - index) >= 0) {
				int_t[offset + index] = Program::DATA[t - index][N];
			} else {
				int_t[offset + index] = Program::DATA[t][N];
			}
		}
		 */

		result = program->execute_program(int_t);

		if (!finite(result)) {
			//std::cout << "El resultado del programa salio mal " << result << "\n";
			error_quad[t] = HUGE_NUMBER;
		} else {
			error_quad[t] = pow((result - Program::DATA[t][config_position]), 2.0);
			if (!finite(error_quad[t])) {
				error_quad[t] = HUGE_NUMBER;
			}
		}

		if (error_quad[t] == 0) {
			/*std::cout << "error_quad[t] Cero\n";
			std::cout << "result " << result << "\n";
			std::cout << "Program::DATA[t][config_position] " << Program::DATA[t][config_position] << "\n";*/

			if (result == 0) {
				print_individual();
			}
		}
		//std::cout << "result: " << result << "   data: " << Program::DATA[t][config_position] << "\n",
		//std::cout << "error: " << error_quad[t] << "\n";
		//std::cout << "********************************************************" << "\n\n";
		//std::cout << "********************************************************" << "\n\n";
		error_a_quad += error_quad[t];
	}

	error_prom_quad = error_a_quad / TRAINING_LINES;

	if (!finite(error_prom_quad)) {
		error_prom_quad = HUGE_NUMBER;
	} else if (error_prom_quad == 0.0) {
		std::cout << "Error Cero\n";
		std::cout << "error_a_quad " << error_a_quad << "\n";
		std::cout << "///Errores\n";
		/*for (int t = 0; t < TRAINING_LINES; t++) {
			std::cout << "error_quad[" << t << "] " << error_quad[t] << "\n";
		}*/
		error_prom_quad = 0.000000001;
	}

	for (int i = 0; i < TRAINING_LINES; i++) {
		error_dev += pow((error_prom_quad - error_quad[i]), 2);
		if (!finite(error_dev)) {
			//std::cout << "Overflow al calcular la desviacion\n";
			error_dev = HUGE_NUMBER;
		}
	}

	error_dev /= (TRAINING_LINES - 1);


	error = error_prom_quad;
	sigma = sqrt(error_dev);

	double weight = ((W_OB1 * error) + (W_OB2 * sigma));
	if (weight != 0) {
		fitness = 1 / weight;
	} else {
		fitness = HUGE_NUMBER;
	}
 	


	if (!finite(fitness)) {
		//std::cout << "********************************************************" << "\n\n";
		if (isnan(fitness)) {
			std::cout << "Fitness NaN" << "\n\n";
			//std::cout << "error: " << error << "\n",
			//std::cout << "sigma: " << sigma << "\n";
			//std::cout << "********************************************************" << "\n\n";
		} else {
			std::cout << "Fitness Inf" << "\n\n";
		}
		//std::cout << "error: " << error << "\n",
		//std::cout << "sigma: " << sigma << "\n";
		//print_individual();
		//std::cout << "********************************************************" << "\n\n";
		fitness = 0.0;
	}

	evaluated = true;
}


inline double Individual::evaluate(int obj = FITNESS) const {
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

	//borrar despues
	if (!x.evaluated || !y.evaluated ) {
		std::cout << "compare_ob1 EVALUAR FITNESS ANTES DE USAR ESTA FUNCION!!!!\n";
		std::cout << "x fitness y n_eff \n";
		std::cout <<  x.fitness << ", ";
		std::cout << x.program->n_eff << "\n";

		std::cout << "y fitness y n_eff \n";
		std::cout <<  y.fitness << ", ";
		std::cout << y.program->n_eff << "\n";
	}
	//hasta aca
	if (x.evaluate() > y.evaluate()) {
		return false;
	} else if (x.evaluate() < y.evaluate()) {
		return true;
	} else if (x.evaluate() == y.evaluate()) {
		return false;
	}
	/*
	std::cout<<"compare_fitness FIN\n";
	std::cout<<"X\n";
	x.print_individual();

	std::cout<<"Y\n";
	y.print_individual();
	*/

}


inline bool Individual::compare_error_prom(Individual &x, Individual &y) {
	if (x.evaluate(OB1) > y.evaluate(OB1)) {
		return false;
	} else if (x.evaluate(OB1) < y.evaluate(OB1)) {
		return true;
	} else if (x.evaluate(OB1) == y.evaluate(OB1)) {
		return false;
	}
}


inline bool Individual::compare_sigma(Individual &x, Individual &y) {
	if (x.evaluate(OB2) > y.evaluate(OB2)) {
		return false;
	} else if (x.evaluate(OB2) < y.evaluate(OB2)) {
		return true;
	} else if (x.evaluate(OB2) == y.evaluate(OB2)) {
		return false;
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


void Individual::check(int deme, int index) {
	std::cout << "From deme: " << deme << " ";
	std::cout << "individidual: " << index << " ";
	std::cout << "fitness: " << fitness << " ";
	std::cout << "error: " << error << " ";
	std::cout << "sigma: " << sigma << "\n";
}

int Individual::manhattan_op_distance(const Individual * ind) {
	int distance = 0;

	for (int i = 1; i <= NUM_OPERATORS; i++) {
		if (program->op_diversity[i] < 0 || ind->program->op_diversity[i] < 0) {
			std::cout << " \n\n\n" << "op_diversity negative !!!!!!!: " << " \n\n\n";
			std::cout << "\n Individuo 1" << " \n";
			for (int j = 1; j <= NUM_OPERATORS; j++) {
					std::cout << "op[" << j <<"]: " << program->op_diversity[j] << " \n";
			}
			std::cout << "\n Individuo 2" << " \n";
			for (int j = 1; j <= NUM_OPERATORS; j++) {
					std::cout << "op[" << j <<"]: " << ind->program->op_diversity[j] << " \n";
			}
		}
		distance += std::abs(program->op_diversity[i] - ind->program->op_diversity[i]);
	}

	if (distance < 0 ) {
		std::cout << " \n\n\n" << "Distance Negative !!!!!!!: " << " \n\n\n";
	}

	return distance;
}

void Individual::print_op_diversity() {

	for (int j = 1; j <= NUM_OPERATORS; j++) {
		std::cout << "op[" << j <<"]: " << program->op_diversity[j];
		if (program->op_diversity[j] < 0) {
			std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@";
		}
		std::cout << "\n";
	}
}


bool Individual::check_op_diversity() {

	for (int j = 1; j <= NUM_OPERATORS; j++) {
		if (program->op_diversity[j] < 0) {
			return true;
		}
	}
	return false;
}

void Individual::print_individual(bool resume) {
	std::cout << "Config Pos: " << config_position << "\n";
	std::cout << "Training error: " << error << "\n";
	std::cout << "Trainig Deviation: " << sigma << "\n";
	std::cout << "Fitness: " << fitness << "\n";
	std::cout << "List Size: " << program->height << "\n";
	std::cout << "List effective Size: " << program->n_eff << "\n";

/*	std::cout << "\n";
	std::cout << "List Instruction: " << "\n";
	for (int i = 0; i < program->height; i++) {
		std::cout << program->list_inst[i].get_str_instruction();
		program->list_inst[i].print_instruction();
	}*/

	if (!resume) {
		std::cout << "\n";
		std::cout << "List Effective: " << "\n";
		for (int i = 0; i < program->n_eff; i++) {
			//std::cout << i << "- ";
			std::cout << program->effective_list_inst[i].get_str_instruction();
		}

		std::cout << "\n";
		//std::cout.precision(15);
		std::cout << "Registers: " << "\n";
		for (int i = 0; i < NUM_INDIVIDUAL_REGISTERS; i++) {
			std::cout << "r_all[" << i << "] = " << program->list_reg[i] << "\n";
		}
	}

}



void Individual::check_max_min_instructions(std::string name,
		std::string lugar) {
	if (program->height > NUM_MAX_INSTRUCTIONS) {
		std::cout << "[Individual::check_max_min_instructions]"
				<< name + " - Superó el número maximo de instrucciones ->"
						+ lugar;
	}
	if (program->height < NUM_MIN_INSTRUCTIONS) {
		std::cout << "[Individual::check_max_min_instructions]"
				<< name + " - Superó el número minimo de instrucciones ->"
						+ lugar;
	}
}


void Individual::crossover(Individual * &genome1, Individual * &genome2) { //, Individual * &child1, Individual * &child2
	#ifdef USING_OMP
	Individual::cant_crossover[omp_get_thread_num()]+=1;
	#endif

	genome1->check_max_min_instructions("genome1", "Antes Crossover");
	genome2->check_max_min_instructions("genome2", "Antes Crossover");
	Individual * mom, *sister;
	Individual * dad, *brother;

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
		min_segment_full_dad = (dad->program->height + mom_segment_size)
				- NUM_MAX_INSTRUCTIONS;

		//lo que falta para completar el mínimo número de instrucciones al mom
		min_segment_size_num_min_in_mom = NUM_MIN_INSTRUCTIONS - (mom->program->height - mom_segment_size);

		if (min_segment_full_dad > min_segment_size_num_min_in_mom) {
			min_segment_size_dad = min_segment_full_dad;
		} else {
			min_segment_size_dad = min_segment_size_num_min_in_mom;
		}

		if (min_segment_size_dad <= 0) {
			//std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++Crossover" << "\n";
			//std::cout << "+++ min " << min_segment_size_dad << "\n";
			//std::cout << "+++ max " << max_segment_size_dad << "\n";
			if (max_segment_size_dad > 1) {
				min_segment_size_dad = 1;
			} else {
				min_segment_size_dad = 0;
			}
		}

		dad_segment_size = randint(min_segment_size_dad, max_segment_size_dad);
		max_padding_dad = dad->program->height - dad_segment_size - 1;

		//desde donde se puede comenzar para que alcancen las instrucciones del segmento a cruzar

		cuts_points_dad[0] = randint(0, max_padding_dad);
		cuts_points_dad[1] = cuts_points_dad[0] + dad_segment_size;

		//Se crean nuevos temporales
		sister = new Individual;
		brother = new Individual;

		//std::cout << "cp mon : " << cuts_points_mom[0] << " , " << cuts_points_mom[1] << "\n";
		//std::cout << "cp dad : " << cuts_points_dad[0] << " , " << cuts_points_dad[1] << "\n";

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

	#ifdef USING_OMP
	//Contador de macromutaciones
	Individual::cant_macro[omp_get_thread_num()]+=1;
	#endif

	bool insertion = random_flip_coin(P_INS);
	int mutation_point = randint(0, program->height - 2);

	//std::cout << "mutation_point: " << mutation_point << "\n";
	if (program->height < NUM_MAX_INSTRUCTIONS && (insertion || program->height == NUM_MIN_INSTRUCTIONS)) {

		#ifdef USING_OMP
		//Contador de inserciones.
		Individual::cant_macro_ins[omp_get_thread_num()]+=1;
		#endif

		// Si no supera la max. cant. de instrucciones y es insercion o tiene el numero minimo de instrucciones
		Instruction new_instruction;
		new_instruction.create_new_instruction();
		std::vector<int> reg_eff;
		int to_mutate = program->get_effective_registers(mutation_point, reg_eff);

		while (reg_eff.empty()) {
			/* se da en el caso de que el punto de mutación esté por debajo de la última
			 * instrucción efectiva y ésta sea unaria con un operando constante
			 * cambiar el registro constante del operador unario por uno variable
			 */
			program->list_inst[to_mutate].op2 = randint(VAR_MIN, VAR_MAX);
			to_mutate = program->get_effective_registers(mutation_point, reg_eff);
		}

		new_instruction.dest = reg_eff.at(randint(0, reg_eff.size() - 1));
		Instruction * new_list = new Instruction[program->height + 1];

		/*
		 * Insertar la nueva instrucciòn en la posición mutation_point
		 */

		//se copia parte 1
		std::copy(program->list_inst, program->list_inst + mutation_point, new_list);
		//Se copia la nueva instruccion
		new_list[mutation_point] = new_instruction;
		//Se copia la parte 2 - se corre uno ya que se remplazo el punto de corte
		std::copy(program->list_inst + mutation_point, program->list_inst + program->height, new_list + mutation_point + 1);

		set_altered();
		program->height++;
		delete [] program->list_inst;
		program->list_inst = new_list;

	} else if (program->height > NUM_MIN_INSTRUCTIONS && (!insertion || program->height == NUM_MAX_INSTRUCTIONS)) {
		//Si es mayor a la  min. cant. de instrucciones y  no es insercion o tiene el numero maximo de instrucciones

		#ifdef USING_OMP
		//Contador de eliminaciones.
		Individual::cant_macro_del[omp_get_thread_num()]+=1;
		#endif

		Instruction * new_list = new Instruction[program->height - 1];
		if (mutation_point != 0) {
			std::copy(program->list_inst, program->list_inst + mutation_point, new_list);
			std::copy(program->list_inst + mutation_point + 1, program->list_inst + program->height, new_list + mutation_point);
		} else {
			std::copy(program->list_inst + 1, program->list_inst + program->height, new_list);
		}
		delete [] program->list_inst;
		program->list_inst = new_list;
		program->height--;
		set_altered();
	}

	if (program->height > NUM_MAX_INSTRUCTIONS) {
		std::cout << "superado el número maximo de instrucciones MACRO";
		std::cout << "genome.height > Parameters.num_max_instructions";
	}
	//std::cout << "ya esta\n";
}



int Individual::get_random_register(int &pos_to_replace, Instruction * instruction = 0,  std::vector<int> reg_eff = { }) {
	int _register = 0;

	switch (pos_to_replace) {
	case (DEST) :
		if (reg_eff.empty()) {
			if (instruction->oper < 5) {
				//se hace que se mute uno de los registros operandos
				pos_to_replace = random_flip_coin(0.5) ? OPERAND_1 : OPERAND_2;
			} else {
				//#operación unaria, se muta el operando 2 para que sea efectiva
				pos_to_replace = OPERAND_2;
			}
		} else {
			int n = randint(0, reg_eff.size() - 1);
			_register = reg_eff.at(n);

			int count = 0;
			while (_register == instruction->dest && count < WHILE_BLOCK) {
				n = randint(0, reg_eff.size() - 1);
				_register = reg_eff.at(n);
				count++;
			}
		}
	case (OPERAND_1) :
		//Cambio: se permite al primer operando ser variable o constante de entrada
		if (random_flip_coin(0.5)) {
			_register = randint(VAR_MIN, VAR_MAX);
		} else {
			if (random_flip_coin(P_REG_CONST_DELTA)) {
				_register = randint(CONST_IN_DELTA_MIN, CONST_IN_DELTA_MAX);

				int count = 0;
				while (_register == instruction->op1 && count < WHILE_BLOCK) {
					_register = randint(CONST_IN_DELTA_MIN, CONST_IN_DELTA_MAX);
					count++;
				}

			} else {
				_register = randint(CONST_IN_MIN, CONST_IN_MAX);

				int count = 0;
				while (_register == instruction->op1 && count < WHILE_BLOCK) {
					_register = randint(CONST_IN_MIN, CONST_IN_MAX);
					count++;
				}
			}
		}
	case (OPERAND_2) :
		//operador 2 es constante con probabilidad p_const
		if (random_flip_coin (P_REG_OP2_CONST)) {
			//Si va a ser constante, las constantes de entrada tienen mayor
			//probabilidad que las aleatorias.
			if (random_flip_coin (P_CONST_IN)) {
				if (random_flip_coin(P_REG_CONST_DELTA)) {
					_register = randint(CONST_IN_DELTA_MIN, CONST_IN_DELTA_MAX);

					int count = 0;
					while (_register == instruction->op2 && count < WHILE_BLOCK) {
						_register = randint(CONST_IN_DELTA_MIN, CONST_IN_DELTA_MAX);
						count++;
					}

				} else {
					_register = randint(CONST_IN_MIN, CONST_IN_MAX);

					int count = 0;
					while (_register == instruction->op2 && count < WHILE_BLOCK) {
						_register = randint(CONST_IN_MIN, CONST_IN_MAX);
						count++;
					}

				}
			} else {
				_register = randint(CONST_AL_MIN, CONST_AL_MAX);

				int count = 0;
				while (_register == instruction->op2 && count < WHILE_BLOCK) {
					_register = randint(CONST_AL_MIN, CONST_AL_MAX);
					count++;
				}
			}
		} else {
			_register = randint(VAR_MIN, VAR_MAX);

			int count = 0;
			while (_register == instruction->op2 && count < WHILE_BLOCK) {
				_register = randint(VAR_MIN, VAR_MAX);
				count++;
			}

		}
	}

	return _register;
}

void Individual::micro_mutation() {
	/* Muta las instrucciones efectivas internamente
	 * p_regmut = probabilidad de mutar un registro
	 * p_opermut = probabilidad de mutar una operación
	 * p_constmut = probabilidad de mutar una constante efectiva
	 * Individual::cant_micro++ Contador de inserciones
	 */
	#ifdef USING_OMP
	Individual::cant_micro[omp_get_thread_num()]+=1;
	#endif

	program->get_effective_instructions();
	int * indices = program->get_effective_instructions_with_indices();
	std::vector<int> v_indices;
	int index = randint(0, program->n_eff - 1);
	int mutation_point = indices[index];
	int pos_to_replace = 0;
	int type = select_micro_mutacion_type(_random());
	int op = -1;
	int ins_with_constant_index, register_mutation_index;
	std::vector<int> constants_indices;
	
	if (type == CONSTANTES) {
		constants_indices = program->get_effective_constant_indices();

		if (!constants_indices.empty()) {
			ins_with_constant_index = constants_indices.at(
					randint(0, constants_indices.size() - 1));
			register_mutation_index =
					program->list_inst[ins_with_constant_index].op2;
			#ifdef USING_OMP
			Individual::cant_micro_const[omp_get_thread_num()]+=1;
			#endif
		} else {
			
			//no hay instrucciones efectivas con registros constantes variables
			type = select_micro_mutacion_type(_random());
			//si vuelve a salir constante, se elige mutación de registro o operaciones con igual probabilidad
			if (type == CONSTANTES) {
				type = random_flip_coin(0.5) ? REGISTROS : OPERACIONES;
			}
			
			//type = OPERACIONES;
		}
	}
	
	if (type == REGISTROS) {
		if (program->n_eff == 1) {
			//una sola instrucción efectiva, no se puede cambiar r[0]
			if (program->list_inst[mutation_point].oper < 5) {
				pos_to_replace = randint(OPERAND_1, OPERAND_2);
			} else { //operación unaria, cambiar el segundo operando
				pos_to_replace = OPERAND_2;
			}
		} else {
			if (program->list_inst[mutation_point].oper < 5)
				pos_to_replace = randint(DEST, OPERAND_2);
			else
				//operación unaria, cambiar el segundo operando o el destino
				pos_to_replace = random_flip_coin(0.5) ? DEST : OPERAND_2;
		}
		
		if (pos_to_replace == DEST) { //destino
			//si no es el último índice, la última

			if ((index + 1) < program->n_eff) {
				//Se obtiene la lista de registros efectivos en esa posición, para reemplazar
				//  en la instrucción y permitir que siga siendo efectiva                 *
				 
				std::vector<int> reg_eff;
				int to_mutate = program->get_effective_registers(indices[index + 1], reg_eff);
				if (!reg_eff.empty()) {
					std::vector<int>::iterator i = std::find(reg_eff.begin(), reg_eff.end(), program->list_inst[mutation_point].dest);
					if (i != reg_eff.end()) {
						reg_eff.erase(i); //remover el registro destino de los registros efectivos
						op = get_random_register(pos_to_replace, &program->list_inst[mutation_point], reg_eff);
					}
				}
			} else { //el punto de mutación es la última instrucción con el r[0]
				if (program->list_inst[mutation_point].oper < 5) {
					pos_to_replace = randint(OPERAND_1, OPERAND_2);
				} else {
					//operación unaria, cambiar el segundo operando
					pos_to_replace = OPERAND_2;
				}
				op = get_random_register(pos_to_replace, &program->list_inst[mutation_point]);
			}
		
		} else { //para los casos de operandos op1 y op2
			op = get_random_register(pos_to_replace, &program->list_inst[mutation_point]);
		}

		switch (pos_to_replace) {
		case (DEST) :
			program->list_inst[mutation_point].dest = op;
			break;
		case (OPERAND_1) :
			program->list_inst[mutation_point].op1 = op;
			break;
		case (OPERAND_2) :
			program->list_inst[mutation_point].op2 = op;
			break;
		}
		#ifdef USING_OMP
		Individual::cant_micro_reg[omp_get_thread_num()]+=1;
		#endif
	}
	
	
	int diff_op = 0;

	if (type == OPERACIONES) {
		diff_op = randint(OP_MIN, OP_MAX);

		while (program->list_inst[mutation_point].oper == diff_op) {
			diff_op = randint(OP_MIN, OP_MAX);
		}

		program->list_inst[mutation_point].oper = diff_op;
		#ifdef USING_OMP
		Individual::cant_micro_ope[omp_get_thread_num()]+=1;
		#endif

	}

	set_altered();
}


int Individual::select_micro_mutacion_type(float prob) {
	if (prob <= P_REGMUT) {
		return REGISTROS;
	} else {
		if (prob > P_REGMUT && prob <= P_OPERMUT + P_REGMUT) {
			return OPERACIONES;
		} else if (prob > (P_OPERMUT + P_REGMUT)
				&& prob <= (P_OPERMUT + P_REGMUT + P_CONSTMUT)) {
			return CONSTANTES;
		}
	}

	return -1;
}


double * Individual::eval_individual(int tipo) {
	program->get_effective_instructions();

	double result;
	double * int_t;
	double error_a_quad, error_prom_quad;
	int size;
	int ini, end;
	int approximation_offset;

	if (tipo == TRAINING) {
		size = TRAINING_LINES;
		ini = 0;
		end = TRAINING_LINES;
	} else if(tipo == VALIDATION) {
		size = VALIDATION_LINES;
		ini = TRAINING_LINES;
		end = LINES;
	}

	double * error_quad = new double[size+3];
	double * approximation = new double[size];

	int index = 0;
	error_a_quad = 0.0;
	for (int t = ini; t < end; t++) {
		int_t = Program::R_CONST[t];

#ifdef Q
		int offset = Q;
#else
		int offset = K;
#endif

		for (int in_index = 1; in_index < DELTA; ++in_index) { // La posición offset está ocupada por el promedio de las mediciones
			if ((t - in_index) >= 0) {
				int_t[offset + in_index] = Program::DATA[t - in_index][N];
			} else {
				int_t[offset + in_index] = Program::DATA[t][N];
			}
		}

		result = program->execute_program(int_t);

/*		if(tipo == VALIDATION) {
			approximation[t - TRAINING_LINES] = result;
		}*/

		if (!finite(result)) {
/*			if(tipo == VALIDATION) {
				approximation[t - TRAINING_LINES] = C_UNDEF;
			}*/
		 	error_quad[index] = HUGE_NUMBER;
		} else {
			error_quad[index] = pow((result - Program::DATA[t][config_position]), 2.0);
			if (!finite(error_quad[index])) {
				error_quad[index] = HUGE_NUMBER;
			}
		}

		//****************************** CALCULAR SUMA & PROM *****************************
		error_a_quad += error_quad[index];
		index++;
	}

	if (!finite(error_a_quad)) {
		error_a_quad = HUGE_NUMBER;
	} else if (error_a_quad == 0.0) {
		std::cout << "Error Cero\n";
		std::cout << "error_a_quad " << error_a_quad << "\n";
		std::cout << "///Errores\n";
		for (int t = ini; t < end; t++) {
			std::cout << "error_quad[" << t << "] " << error_quad[t] << "\n";
		}
		error_a_quad = 0.000000001;
	}

	error_prom_quad = error_a_quad / size;

	error_quad[size] = error_a_quad;
	error_quad[size + 1] = error_prom_quad;
	error_quad[size + 2] = config_position;
	/*
	if (tipo == VALIDATION) {
		std::cout << "Approximations en Validacion\n";
		for (int h = 0; h < size; h++) {
			std::cout << "[" << (h + TRAINING_LINES) << "] " << approximation[h] << "\n";
		}
		std::cout << "-------------------------------\n";
	}
	*/
	delete [] approximation;

	return error_quad;
}

double * Individual::eval_individual_result(int tipo) {
	program->get_effective_instructions();

	double result;
	double * int_t;
	double error_a_quad, error_prom_quad;
	int size;
	int ini, end;
	int approximation_offset;

	if (tipo == TRAINING) {
		size = TRAINING_LINES;
		ini = 0;
		end = TRAINING_LINES;
	} else if(tipo == VALIDATION) {
		size = VALIDATION_LINES;
		ini = TRAINING_LINES;
		end = LINES;
	}

	double * error_quad = new double[size+3];
	double * approximation = new double[size];

	int index = 0;
	error_a_quad = 0.0;
	for (int t = ini; t < end; t++) {
		int_t = Program::R_CONST[t];

#ifdef Q
		int offset = Q;
#else
		int offset = K;
#endif

		for (int in_index = 1; in_index < DELTA; ++in_index) { // La posición offset está ocupada por el promedio de las mediciones
			if ((t - in_index) >= 0) {
				int_t[offset + in_index] = Program::DATA[t - in_index][N];
			} else {
				int_t[offset + in_index] = Program::DATA[t][N];
			}
		}

		result = program->execute_program(int_t);

/*		if(tipo == VALIDATION) {
			approximation[t - TRAINING_LINES] = result;
		}*/

		if (!finite(result)) {
/*			if(tipo == VALIDATION) {
				approximation[t - TRAINING_LINES] = C_UNDEF;
			}*/
		 	error_quad[index] = C_UNDEF;
		} else {
			error_quad[index] = result;
			if (!finite(error_quad[index])) {
				error_quad[index] = C_UNDEF;
			}
		}

		//****************************** CALCULAR SUMA & PROM *****************************
		error_a_quad += error_quad[index];
		index++;
	}

	if (!finite(error_a_quad)) {
		error_a_quad = HUGE_NUMBER;
	} else if (error_a_quad == 0.0) {
		std::cout << "Error Cero\n";
		std::cout << "error_a_quad " << error_a_quad << "\n";
		std::cout << "///Errores\n";
		for (int t = ini; t < end; t++) {
			std::cout << "error_quad[" << t << "] " << error_quad[t] << "\n";
		}
		error_a_quad = 0.000000001;
	}

	error_prom_quad = error_a_quad / size;

	error_quad[size] = error_a_quad;
	error_quad[size + 1] = error_prom_quad;
	error_quad[size + 2] = config_position;
	/*
	if (tipo == VALIDATION) {
		std::cout << "Approximations en Validacion\n";
		for (int h = 0; h < size; h++) {
			std::cout << "[" << (h + TRAINING_LINES) << "] " << approximation[h] << "\n";
		}
		std::cout << "-------------------------------\n";
	}
	*/
	delete [] approximation;

	return error_quad;
}


