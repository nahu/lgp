/*
 * individual.h
 *
 *  Created on: Mar 16, 2013
 *      Author: Vanessa Cañete, Nahuel Hernández
 */



class Individual {
public:
	Individual(int config_position, int index);
	Individual();
	~Individual();
	//Evaluacion de fitness
	void eval_fitness();
	inline void set_altered();
	double evaluate(int obj);

	//operadores genéticos
	void check_max_min_instructions (std::string name, std::string place);
	static void crossover(Individual &genome1, Individual &genome2, Individual &sister, Individual &brother);
	void exchange(Individual * mom, Individual * dad, int * cuts_points_mom, int * cuts_points_dad);
	void clone(Individual * orig);
	void print_individual();

	void macro_mutation ();
	void micro_mutation ();
	std::string select_micro_mutacion_type(float prob);
	int get_random_register(int &op, std::vector<int> reg_eff, Instruction * instruction);

	Program * program = 0;
	double fitness;
	double error;
	double sigma; //era dev
	int index;
	int config_position; //ver si hace falta
	bool evaluated;
	//pasar todo lo que esta en la clase individuo
	//poner acá los operadores genéticos
};

Individual::Individual() {
	fitness = 0.0;
	error = 0.0;
	sigma = 0.0;
	index = -1;
	config_position = -1;
	program = 0;
	evaluated = false;
}

Individual::Individual(int config_position, int index) {
	fitness = 0.0;
	error = 0.0;
	sigma = 0.0;
	this->index = index;
	this->config_position = config_position;
	evaluated = false;
	program = new Program;
}

Individual::~Individual() {
	delete program;
}

inline void Individual::set_altered() {
	fitness = 0.0;
	evaluated = false;
}

void Individual::eval_fitness() {
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


double Individual::evaluate(int obj=FITNESS) {
	if (!evaluated) {
		eval_fitness();
	}

	if (obj == FITNESS) {
		return fitness;
	} else if (obj == OB1) {
		return error;
	} else if (obj == OB2) {
		return sigma;
	}
}

void Individual::print_individual() {
	std::cout << "Index: " << index << "\n";
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
	std::cout << "Registers: " << index << "\n";
	for (int i = 0; i < NUM_INDIVIDUAL_REGISTERS; i++) {
		std::cout <<  "r_all[" << i << "] = " << program->list_reg[i] << "\n";
	}

}

void Individual::clone(Individual * orig){
	fitness = orig->fitness;
	error = orig->error;
	sigma = orig->sigma;
	index = orig->index;
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

void Individual::exchange(Individual * g1, Individual * g2, int g1_cuts_p [2], int g2_cuts_p [2]){
	/* Se remplaza el bloque 2 por el bloque 4.
	 *
	 * |----------|***|--------|  <-     |---|*****|----------------| = |----------|*****|--------|
	 * |    1     | 2 |    3   |  <-     | 3 |  4  |        5       |   |    1     |  4  |    3   |
	 */
	//se calcula la nueva longitud = len(1) + len(4) + len(3)
	int new_len = g1_cuts_p[0];
	new_len += (g2_cuts_p[1] - g2_cuts_p[0]) + 1;
	new_len += (g1->program->height - g1_cuts_p[1]) - 1;
	//se borra la lista actual
	delete [] program->list_inst;
	//Se crea una nueva lista
	program->list_inst = new Instruction[new_len];
	program->height = new_len;
	//Se copia parte 1
	int i = 0;
	for (i = 0; i < new_len; i++){
		program->list_inst[i].oper= 0;
		program->list_inst[i].dest= 0;
		program->list_inst[i].op1 = 0;
		program->list_inst[i].op2 = 0;
	}
	/* borrar desde aca*/
	/*std::cout << "***************************************"<<"\n";
	std::cout << "LISTA DE INSTRUCCIONES DE G1" << "\n";
	Program::print_list_int(g1->program->list_inst, g1->program->height);
	std::cout << "CUTS POINTS DE G1"<<"\n";
	std::cout << g1_cuts_p[0] << "  " << g1_cuts_p[1]<<"\n";

	std::cout << "LISTA DE INSTRUCCIONES DE G2" << "\n";
	Program::print_list_int(g2->program->list_inst, g2->program->height);
	std::cout << "CUTS POINTS DE G2"<<"\n";
	std::cout << g2_cuts_p[0] << "  " << g2_cuts_p[1]<<"\n";

	std::cout << "LISTA DE INSTRUCCIONES ACTUAL" << "\n";
	Program::print_list_int(program->list_inst, program->height);

	std::cout<<"Tamanhos:"<<new_len<<"\n parte 1="<<g1_cuts_p[0]<<
		"\n parte 2 = "<< g2_cuts_p[1] - g2_cuts_p[0] + 1<<
		"\n parte 3= " << (g1->program->height - g1_cuts_p[1] - 1)<<"\n";
	std::cout << "***************************************"<<"\n";*/
	/* hasta aca*/

	int pos = 0;
	//se copia parte 1
	for (i = 0; i < g1_cuts_p[0]; i++){
		program->list_inst[pos] = g1->program->list_inst[i];
		pos++;
	}
	//Se copia la parte 4 - incluye ambos limites
	for (i = g2_cuts_p[0]; i <= g2_cuts_p[1] ; i++){
		program->list_inst[pos] = g2->program->list_inst[i];
		pos++;
	}
	//Se copia la parte 2 - se corre uno ya que se remplazo el punto de corte
	for (i = g1_cuts_p[1]+1; i < g1->program->height ; i++){
		program->list_inst[pos] = g1->program->list_inst[i];
		pos++;
	}
	set_altered();
}
void Individual::crossover(Individual &genome1, Individual &genome2, Individual &sister, Individual &brother) {
	genome1.check_max_min_instructions("genome1", "Antes Crossover");
	genome2.check_max_min_instructions("genome2", "Antes Crossover");
	Individual  dad, mom;
	int cuts_points_mom [2]= {0,0};
	int cuts_points_dad [2]= {0,0};

	if(genome1.program->height >= genome2.program->height ){
		mom.clone(&genome2);
		dad.clone(&genome1);
	}else{
		mom.clone(&genome1);
		dad.clone(&genome2);
	}
	try{
		int max_segment_size_mom = mom.program->height - 1;
		//al menos 1 instruccion menos la ultima.
		int mom_segment_size = randint(1, max_segment_size_mom);
		int max_padding_mom = mom.program->height - mom_segment_size - 1;
		//desde donde se puede comenzar para que alcancen las instrucciones del segmento a cruzar
		cuts_points_mom[0] = randint(0, max_padding_mom);
		cuts_points_mom[1] = cuts_points_mom[0] + mom_segment_size;

		//para que no incluya a la ultima instruccion.
		cuts_points_mom[1] = (cuts_points_mom[1] == mom.program->height-1)? cuts_points_mom[1] - 1 :cuts_points_mom[1] ;

		//el máximo segmento a cruzar es lo que le falta al mom para completar el máximo número de instrucciones permitidas
		int max_segment_full_mom = NUM_MAX_INSTRUCTIONS - (mom.program->height - mom_segment_size);
		int max_segment_num_min_in_dad = (dad.program->height + mom_segment_size) - NUM_MIN_INSTRUCTIONS;
		//se elije el menor de los máximos
        int max_segment_size_dad = max_segment_full_mom < max_segment_num_min_in_dad ? max_segment_full_mom : max_segment_num_min_in_dad;

        //si el maximo es mayor a la longitud del padre, se elige la longitud como máximo
        max_segment_size_dad = (dad.program->height - 1) < max_segment_size_dad ? (dad.program->height - 1) : max_segment_size_dad;

        //lo que hay que quitarle para que quede en máximo número de instrucciones
        int min_segment_full_dad = (dad.program->height + mom_segment_size) - NUM_MAX_INSTRUCTIONS;

        //lo que falta para completar el mínimo número de instrucciones al mom
        int min_segment_size_num_min_in_mom = NUM_MIN_INSTRUCTIONS - (mom.program->height - mom_segment_size);

        int min_segment_size_dad = (min_segment_full_dad > min_segment_size_num_min_in_mom) ? min_segment_full_dad : min_segment_size_num_min_in_mom;
        min_segment_size_dad = min_segment_size_dad <= 0 ? 1 :min_segment_size_dad;

        int dad_segment_size = randint(min_segment_size_dad, max_segment_size_dad);
        int max_padding_dad = dad.program->height - dad_segment_size - 1;

        //desde donde se puede comenzar para que alcancen las instrucciones del segmento a cruzar

		cuts_points_dad[0] = randint(0, max_padding_dad);
		cuts_points_dad[1] = cuts_points_dad[0] + dad_segment_size - 2;
		//para que no incluya a la ultima instruccion.
		cuts_points_dad[1] = (cuts_points_dad[1] == dad.program->height-1) ?cuts_points_dad[1] - 1 : cuts_points_dad[1];

		//Se clonan los individuos
        sister.clone(&mom);
        brother.clone(&dad);
        //Se intercambian los bloques
        sister.exchange(&mom, &dad, cuts_points_mom, cuts_points_dad);
        brother.exchange(&dad, &mom, cuts_points_dad, cuts_points_mom);
        sister.index = genome1.index;
        brother.index = genome2.index;

        sister.check_max_min_instructions("sister", "Despues Crossover");
        brother.check_max_min_instructions("brother", "Despues Crossover");
	}catch (std::exception e) {
		std::cout << "Error en Crossover";
		std::cout<< e.what();

	}
}

void Individual::macro_mutation (){
	//Agrega o quita instrucciones  -- Alg. 6.1 -- p_ins > p_del //
    int insertion = random_flip_coin(P_INS);
    int mutation_point = randint(0, program->height - 2);
    if (program->height < NUM_MAX_INSTRUCTIONS && (insertion || program->height == NUM_MIN_INSTRUCTIONS)){
        // Si no supera la max. cant. de instrucciones y es insercion o tiene el numero minimo de instrucciones
        Instruction new_instruction;
        std::vector<int> reg_eff;
        int to_mutate = program->get_effective_registers(mutation_point,reg_eff);
        while (reg_eff.empty()) {
        	 /* se da en el caso de que el punto de mutación esté por debajo de la última
             * instrucción efectiva y ésta sea unaria con un operando constante
             * cambiar el registro constante del operador unario por uno variable
             */
            program->list_inst[to_mutate].op2 = randint(VAR_MIN, VAR_MAX);
            to_mutate  = program->get_effective_registers(mutation_point, reg_eff);
        }

        new_instruction.oper = reg_eff.at(randint(0, reg_eff.size() - 1));
        Instruction * new_list = new Instruction[program->height + 1];

        /*
         * Insertar la nueva instrucciòn en la posición mutation_point
         */
		int pos = 0;
		int i = 0;
		//se copia parte 1
		for (i = 0; i < mutation_point; i++){
			new_list[pos] = program->list_inst[i];
			pos++;
		}
		//Se copia la parte 4 - incluye ambos limites
		new_list[pos] = new_instruction;
		pos++;
		//Se copia la parte 2 - se corre uno ya que se remplazo el punto de corte
		for (i = mutation_point; i < program->height; i++){
			new_list[pos] = program->list_inst[i];
			pos++;
		}
		set_altered();
        program->height += 1;
        program->list_inst = new_list;
	}
    if (program->height > NUM_MIN_INSTRUCTIONS && (!insertion || program->height == NUM_MAX_INSTRUCTIONS)){
    	//Si es mayor a la  min. cant. de instrucciones y  no es insercion o tiene el numero maximo de instrucciones
    	int i = 0;
        for (i = mutation_point; i < program->height - 1; i++){
			program->list_inst[i] = program->list_inst[i+1];
		}
    	program->height -= 1;
    	set_altered();
    }
    if (program->height > NUM_MAX_INSTRUCTIONS){
    	std::cout<< "superado el número maximo de instrucciones MACRO";
        std::cout<< "genome.height > Parameters.num_max_instructions";
    }
}

int Individual::get_random_register(int &pos_to_replace, std::vector<int> reg_eff = {}, Instruction * instruction = 0){
    int _register = 0;
    std::cout<<"##PROBAR!!!! \nget_random_register: pos to replace "<<pos_to_replace<<"\n";
    if (pos_to_replace == 1){ //destino
    	std::cout<<"***************** SE CAMBIA OPERADOR *******************\n\n";
        if (reg_eff.empty()){
            if (instruction->oper < 5){
            	//se hace que se mute uno de los registros operandos
            	pos_to_replace = random_flip_coin(0.5) ? 2 : 3;
            }else{
            	//#operación unaria, se muta el operando 2 para que sea efectiva
            	pos_to_replace = 3;
            }
        }else{
        	int n = randint(0, reg_eff.size()-1);
            _register = reg_eff.at(n);
        }
    }
    if (pos_to_replace == 2){ //operando 1
        //Cambio: se permite al primer operando ser variable o constante de entrada
        if (random_flip_coin(0.5))
            _register = randint(VAR_MIN, VAR_MAX);
        else
            _register = randint(CONST_IN_MIN, CONST_IN_MAX);
        _register = randint(VAR_MIN, VAR_MAX);
    }
    if (pos_to_replace == 3) {//operando 2
        //operador 2 es constante con probabilidad p_const
        if (random_flip_coin(P_REG_OP2_CONST)){
            //Si va a ser constante, las constantes de entrada tienen mayor
            //probabilidad que las aleatorias.
            if (random_flip_coin(P_CONST_IN))
                _register = randint(CONST_IN_MIN, CONST_IN_MAX);
            else
                _register = randint(CONST_AL_MIN, CONST_AL_MAX);
        }else{
            _register = randint(VAR_MIN, VAR_MAX);
        }
    }
    return _register;
}

void Individual::micro_mutation (){
    /* Muta las instrucciones efectivas internamente
     * p_regmut = probabilidad de mutar un registro
     * p_opermut = probabilidad de mutar una operación
     * p_constmut = probabilidad de mutar una constante efectiva
     */
    Instruction * eff = program->get_effective_instructions();
    std::cout<<"Lista de registros efectivos"<<"\n";
    Program::print_list_int(eff, program->n_eff);
    std::cout<<"Antes de get_effectiv_inst_with_ind"<<"\n";
    int * indices = program->get_effective_instructions_with_indices();
    std::cout<<"Despues de get_effect.withindices"<<"\n";
    std::vector<int> v_indices;
    std::cout<<"randint"<<"\n";
    int index = randint(0, program->n_eff - 1);
    std::cout<<"mutationpoint "<<indices[index]<<"\n";
    int mutation_point = indices[index];
    int pos_to_replace = 0 ;
    std::cout<<"antes de select_micro_mutacion_type"<<"\n";
    std::string type = select_micro_mutacion_type(_random());
    std::cout<<"despues de select_micro_mutacion_type"<<"\n";
    int op = -1;
    std::cout<<"Tipo de mutacion: "<<type<<"\n";
    type = "constantes";
    if (type == "constantes"){
    	std::cout<<"Constantes> program->get_effective_constant_indices()"<<"\n";
        std::vector<int> constants_indices = program->get_effective_constant_indices();
        std::cout<<"Constantes despues > program->get_effective_constant_indices()"<<"\n";
        //todo: No guarda en constants_indices los indices *aveces
        std::cout<<"size "<<constants_indices.size();
        if (!constants_indices.empty()){
            int ins_with_constant_index = constants_indices.at(randint(0,constants_indices.size()-1));
            int register_mutation_index = program->list_inst[ins_with_constant_index].op2;
            program->list_reg[register_mutation_index] += pow((-1),(randint(0, 1)) * randfloat(0.0, STEP_SIZE_CONST));
        }else{ //no hay instrucciones efectivas con registros constantes variables
            type = select_micro_mutacion_type(_random());
            //si vuelve a salir constante, se elige mutación de registro o operaciones con igual probabilidad
            std::cout<<"si vuelve a salir constante, se elige mutación de registro o operaciones con igual probabilidad\n";
            if (type == "constantes"){
                type = random_flip_coin(0.5) ? "registros" :"operaciones";
                std::cout<<"NUEVO TIPO DE MUTACCION> "<<type<<"\n";
            }
        }
    }

    if (type == "registros"){
        if (program->n_eff== 1){
        	std::cout<<"registros > n_eff ==1 > una sola instrucción efectiva, no se puede cambiar r[0] \n";
        	//una sola instrucción efectiva, no se puede cambiar r[0]
            if (program->list_inst[mutation_point].oper< 5){
                pos_to_replace = randint(2, 3);
                std::cout<<"No es unaria \n	";
            }
            else{ //operación unaria, cambiar el segundo operando
            	std::cout<<"operación unaria, cambiar el segundo operando\n";
                pos_to_replace = 3;
            }
        }else{
            if (program->list_inst[mutation_point].oper < 5)
                pos_to_replace = randint(1, 3);
            else //operación unaria, cambiar el segundo operando o el destino
                pos_to_replace = random_flip_coin(0.5) ? 1 : 3;
        }
        std::cout<<"registros> pos_to_replace> "<<pos_to_replace<<"\n";
        if (pos_to_replace == 1){ //destino
            //si no es el último índice, la última

            if ((index + 1) < program->n_eff){
                /*Se obtiene la lista de registros efectivos en esa posición, para reemplazar
                 * en la instrucción y permitir que siga siendo efectiva                 *
                 */
            	std::vector<int> reg_eff;
            	int to_mutate = program->get_effective_registers(indices[index + 1], reg_eff);
                if (!reg_eff.empty()){
                	std::vector<int>::iterator i = std::find(reg_eff.begin(), reg_eff.end(), program->list_inst[mutation_point].dest);
                	if (i!=reg_eff.end()){
                		reg_eff.erase(i); //remover el registro destino de los registros efectivos
                		op = get_random_register(pos_to_replace, reg_eff, &program->list_inst[mutation_point]);
                	}
                }
            }else { //el punto de mutación es la última instrucción con el r[0]
            	if (program->list_inst[mutation_point].oper < 5)
                    pos_to_replace = randint(2, 3);
                else //operación unaria, cambiar el segundo operando
                    pos_to_replace = 3;
                op = get_random_register(pos_to_replace);
            }
        }else{//para los casos de operandos op1 y op2
        	op = get_random_register(pos_to_replace);
        }
        if(pos_to_replace == 1) program->list_inst[mutation_point].dest = op;
        if(pos_to_replace == 2) program->list_inst[mutation_point].op1  = op;
        if(pos_to_replace == 3) program->list_inst[mutation_point].op2  = op;
    }
    if (type == "operaciones"){
        int diff_op = randint(OP_MIN, OP_MAX);
        while (program->list_inst[mutation_point].oper == diff_op)
            diff_op = randint(OP_MIN, OP_MAX);
        program->list_inst[mutation_point].oper = diff_op;
    }
    set_altered();
}

std::string Individual::select_micro_mutacion_type(float prob){
    if (prob <= P_REGMUT){
        return "registros";
    }else{
    	if (prob > P_REGMUT && prob <= P_OPERMUT + P_REGMUT){
    		return "operaciones";
    	}else if (prob > (P_OPERMUT + P_REGMUT) && prob <= (P_OPERMUT+ P_REGMUT + P_CONSTMUT)){
    		return "constantes";
    	}
    }
    return "";
}
/* TODO: FALTAN LOS SIGUIENTES METODOS
 * def micro_mutation(genome):
 * def select_micro_mutacion_type(prob):
 * */

/*


Individual::
def get_effective_instructions(self):
        //
        intructions[i][0] = identificador de instucción
        intructions[i][1] = registro destino
        intructions[i][2] = registro operando 1
        intructions[i][3] = registro operando 2
        //
        reg_eff = set([0])
        eff_i = []
        for i in reversed(self.genomeList):
            if (i[1] in reg_eff):
                reg_eff.remove(i[1])

                if (i[0] < 5): #los operadores unarios tiene identificador del 5 al 9
                    reg_eff.add(i[2])

                if (i[3] <= Parameters.var_max): #los registros constantes no pueden ser registros efectivos
                    reg_eff.add(i[3])

                eff_i.append(i)

        '''
        Se setea un atributo con el número de instrucciones efectivas del individuo
        '''
        self.n_eff = len(eff_i)
        eff_i.reverse()

        return eff_i


    def get_effective_instructions_with_indices(self):
        reg_eff = set([0])
        eff_i = []
        indices = []
        current_pos = len(self.genomeList)
        for i in reversed(self.genomeList):
            current_pos -= 1
            if (i[1] in reg_eff):
                reg_eff.remove(i[1])

                if (i[0] < 5): #los operadores unarios tiene identificador del 5 al 9
                    reg_eff.add(i[2])

                if (i[3] <= Parameters.var_max): #los registros constantes no pueden ser registros efectivos
                    reg_eff.add(i[3])

                eff_i.append(i)
                indices.append(current_pos)

        eff_i.reverse()
        indices.reverse()
        return eff_i, indices


    def get_effective_constant_indices(self):
        reg_eff = set([0])
        indices = []
        current_pos = len(self.genomeList)
        for i in reversed(self.genomeList):
            current_pos -= 1
            if (i[1] in reg_eff):
                reg_eff.remove(i[1])

                if (i[0] < 6): #los operadores unarios tiene identificador del 5 al 9
                    reg_eff.add(i[2])

                if (i[3] <= Parameters.var_max): #los registros constantes no pueden ser registros efectivos
                    reg_eff.add(i[3])

                elif(i[3] <= Parameters.cons_al_max):
                    indices.append(current_pos)

        return indices


    def get_effective_registers(self, position):
        //
        intructions[i][0] = identificador de instucción
        intructions[i][1] = registro destino
        intructions[i][2] = registro operando 1
        intructions[i][3] = registro operando 2
        //
        reg_eff = set([0])
        current_pos = len(self.genomeList)
        for i in reversed(self.genomeList):
            current_pos -= 1
            if (i[1] in reg_eff):
                reg_eff.remove(i[1])

                if (i[0] < 5):# los operadores unarios tiene identificador del 5 al 9
                    if (i[2] <= Parameters.var_max):
                        reg_eff.add(i[2])

                if (i[3] <= Parameters.var_max): #los registros constantes no pueden ser registros efectivos
                    reg_eff.add(i[3])

            if current_pos == position or not reg_eff:
                return list(reg_eff), current_pos


    def get_program_in_python(self):
        eff_instructions = self.get_effective_instructions()
        program = ""
        for i in eff_instructions:
            '''
            Como los registros de entrada se encuentra en una lista separada al conjunto de registros r_all
            Se realiza un corrimiento de Parameters.register_offset (tamaño de r_all)
            al registro de la instrucción para que entre en el rango de r_const
            variable  variable  - sin corrimiento
            entrada  entrada    - << 4
            variable entrada    - << 8
            entrada variable    - <<11
            '''
            i0 = i[0]
            i2 = i[2] - Parameters.register_offset if i[2] >= Parameters.register_offset else i[2]
            i3 = i[3] - Parameters.register_offset if i[3] >= Parameters.register_offset else i[3]
            ''' Para operaciones de dos operandos'''
            if (i[0]<5):
                if (i[2] >= Parameters.register_offset): #i2 es de entrada
                    if (i[3] >= Parameters.register_offset): #i3 es de entrada
                        i0 = i[0]<<4 #entrada  entrada   - << 4
                    else:
                        i0 = i[0]<<11 #entrada variable  - << 11
                else:
                    if i[3] >= Parameters.register_offset:
                        i0 = i[0]<<8 #variable entrada    - << 8

            else:
                if (i[3] >= Parameters.register_offset):
                    i0 = i[0]<<4 #entrada  entrada   - << 4
            program += (Parameters.operations[i0].format(i[1], i2, i3) + '\n')
        return program


    def eval_fitness(self):
        //
        Función de evaluación de fitnes
        //
        program = self.get_program_in_python()
        list_errors = []
        error_a_quad = 0.0
        error_prom_quad = 0.0

        try:
            for t in range(0, Parameters.training_lines):
                #in_t tiene las mediciones en el instante t
                in_t = Parameters.r_const[t]
                r_all = copy.copy(self.r_all)
                exec program
                error_t = (r_all[0] - Parameters.data_samples[t][self.config_position]) ** 2
                error_a_quad += error_t
                list_errors.append(error_t)

            error_prom_quad = error_a_quad / Parameters.training_lines

            '''Se halla la desviacion tipica del error. (Sumatoria(error_medio - errorXLinea)/training_lines'''
            error_desv = 0.0

            for error_t in list_errors:
                error_desv += (error_prom_quad - error_t) ** 2
            error_desv /= (Parameters.training_lines - 1)

            #para evitar la división por cero
            if error_prom_quad == 0.0:
                error_prom_quad = 0.000000001

            self.error = error_prom_quad
            self.dev = math.sqrt(error_desv)

            self.fitness = 1 / ((Parameters.w_ob1 * error_prom_quad) + (Parameters.w_ob2 * self.dev))

        except Exception as e:
            //
            Si ocurre una excepción el fitness se iguala a cero (Overflow muy probablemente)
            //
            print "EXCEPCION"
            print e
            print program
            print self
            print "error_prom_quad " + str(error_prom_quad)
            print "error_a_quad " + str(error_a_quad)
            self.fitness = 0.0

        self.evaluated = True


    def evaluate(self, obj=0):
        if not self.evaluated:
            self.eval_fitness()

        if (obj == 0):
            return self.fitness
        elif (obj == 1):
            return self.error
        elif (obj == 2):
            return self.dev

        print "ERROR, OBJETIVO NO ENCONTRADO"


    def eval_validation(self):
        //
        Función de evaluación, dado un objeto con el atributo config_position
        retorna una lista del error cuadrático
        para cada línea del los casos de evaluación
        //

        program = self.get_program_in_python()
        #in_t tiene las mediciones en el instante t
        #error_a_quad = []
        error_a_quad = 0.0
    #        print Parameters.r_const
        try:
            for t in range(Parameters.training_lines, Parameters.lines):
                in_t = Parameters.r_const[t]
                r_all = copy.copy(self.r_all)

                exec program

                #error_a_quad.append((r_all[0] - Parameters.data_samples[t][self.config_position]) ** 2)
                error_a_quad += (r_all[0] - Parameters.data_samples[t][self.config_position]) ** 2

            error_prom_quad = error_a_quad / Parameters.validation_lines
        except:
            error_prom_quad = 999999999

        self.validation_error = error_prom_quad

        return error_prom_quad


    def __repr__(self):
        // Return a string representation of Genome //
        ret = "#Index: %s\n" % (self.index)
        ret += "#Config Position: %s\n" % (self.config_position)
        ret += "#Training Error: %s\n" % (self.error)
        ret += "#Training Deviation: %s\n" % (self.dev)
        ret += "#List size:\t %s\n" % (self.height)
#        ret += "#List:\n"


#        ret += "genome_list = [ \n"
#        for line in range(self.height):
#            ret += ""
#            if line < (self.height - 1):
#                item = str(self.genomeList[line])
#                ret += "%s,\n" % (item)
#            elif line == (self.height - 1):
#                ret += "%s]\n\n" % (item)
#
#        ret += "#Registers:\n"
#        for line in range(len(self.r_all)):
#            ret += "r_all[%s] = " % (line)
#            ret += str(self.r_all[line])
#            ret += "\n"
#        ret += "\n"
        ret += "# Constant Registers:\n"
        offset = Parameters.num_var_register
        for line in range(Parameters.num_const_random_registers):

            ret += "r_all[%s] = " % (offset + line + 1)
            ret += str(self.r_all[offset + line + 1])
            ret += "  -  "
        ret += "\n"
        ret+= "#Fitness:\t %.15f\n\n" % (self.fitness)

        return ret


    def clone(self):
        newcopy = copy.deepcopy(self)
        return newcopy


    def set_altered(self):
        self.fitness = 0.0
        self.evaluated = False

    def set_index (self, index):
        self.index = index
        return self
*/
