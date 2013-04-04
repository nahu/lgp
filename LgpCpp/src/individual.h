/*
 * individual.h
 *
 *  Created on: Mar 16, 2013
 *      Author: Vanessa Cañete, Nahuel Hernández
 */
#include<exception>

using namespace std;


class Individual {
public:
	Individual(int, int);
	~Individual();
	void eval_fitness();
	inline void set_altered();

	//operadores genéticos
	static void select_mom_dad(Individual genome1, Individual genome2, Individual * mom, Individual * dad);
	void check_max_min_instructions (string name, string place);
	static void crossover(Individual genome1, Individual genome2, Individual sister, Individual brother);
	void exchange(Individual * mom, Individual * dad, int * cuts_points_mom, int * cuts_points_dad);
	Individual * clone();
	void print_individual();

	Program program;
	double fitness;
	double error;
	double sigma; //era dev
	int index;
	int config_position; //ver si hace falta
	bool evaluated;
	//pasar todo lo que esta en la clase individuo
	//poner acá los operadores genéticos
};

Individual::Individual(int _index, int _config_position) {
	fitness = 0.0;
	error = 0.0;
	sigma = 0.0;
	index = _index;
	config_position = _config_position;
	evaluated = false;
}

Individual::~Individual() {
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

	program.get_effective_instructions();
	std::cout << "Errores en entrenamiento: \n";

	for (int t = 0; t < TRAINING_LINES; t++) {
		double * int_t = Program::R_CONST[t];
		double result = program.execute_program(int_t);
		error_quad[t] = pow((result - Program::DATA[t][config_position]), 2.0);
		std::cout << "result: " << result << "   data: " << Program::DATA[t][config_position] << "\n",
		std::cout << "error: " << error_quad[t] << "\n";
		error_a_quad += error_quad[t];
		std::cout << "********************************************************" << "\n\n";
		std::cout << "********************************************************" << "\n\n";
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

void Individual::print_individual() {

	std::cout << "Index: " << index << "\n";
	std::cout << "Config Pos: " << config_position << "\n";
	std::cout << "Training error: " << error << "\n";
	std::cout << "Trainig Deviation: " << sigma << "\n";
	std::cout << "Fitness: " << fitness << "\n";
	std::cout << "List Size: " << program.height << "\n";
	std::cout << "List effective Size: " << program.n_eff << "\n";
/*
	std::cout <<  "\n";
	std::cout << "List Instruction: " << "\n";
	for (int i = 0; i < program.height; i++) {
		program.list_inst[i].print_instruction();
	}
	*/
	std::cout <<  "\n";
	std::cout << "List Effective: " << "\n";
	for (int i = 0; i < program.n_eff; i++) {
		//std::cout << i << "- ";
		program.effective_list_inst[i].print_instruction();
	}

	std::cout <<  "\n";
	//std::cout.precision(15);
	std::cout << "Registers: " << index << "\n";
	for (int i = 0; i < NUM_INDIVIDUAL_REGISTERS; i++) {
		std::cout <<  "r_all[" << i << "] = " << program.list_reg[i] << "\n";
	}

}

Individual * Individual::clone(){
     Individual *copy = new Individual(index, config_position);
     *copy = (*this);
     return copy;
}
void Individual::check_max_min_instructions (string name, string lugar){
	if (program.height> NUM_MAX_INSTRUCTIONS) {
       std::cout<< name + " - Superó el número maximo de instrucciones ->" + lugar;
	}
    if (program.height < NUM_MIN_INSTRUCTIONS) {
       std::cout<< name + " - Superó el número minimo de instrucciones ->" + lugar;
    }
}
void Individual::select_mom_dad(Individual genome1, Individual genome2, Individual * mom, Individual * dad) {
    mom = genome1.program.height < genome2.program.height  ? genome1.clone() : genome2.clone();
    dad = genome1.program.height >= genome2.program.height ? genome2.clone() : genome1.clone();
}

void Individual::exchange(Individual * g1, Individual * g2, int g1_cuts_p [2], int g2_cuts_p [2]){
	/* Se remplaza el bloque 2 por el bloque 4.
	 *
	 * |----------|***|--------|  <-     |---|*****|----------------| = |----------|*****|--------|
	 * |    1     | 2 |    3   |  <-     | 3 |  4  |        5       |   |    1     |  4  |    3   |
	 */

	//se calcula la nueva longitud = len(1) + len(4) + len(3)
	int new_len = (g1->program.height - g1_cuts_p[0]) + (g2_cuts_p[1] - g2_cuts_p[0]) + (g1->program.height - g1_cuts_p[1]);
	delete [] program.list_inst;
	//Se crea una nueva lista
	Instruction new_list [new_len];
	//Se copia parte 1
	int i = 0;
	for (i = 0; i < g1_cuts_p[0]; i++){
		new_list[i] = g1->program.list_inst[i];
	}
	//Se copia la parte 4
	for (i = g2_cuts_p[0]; i < g2_cuts_p[1]; i++){
		new_list[i] = g2->program.list_inst[i];
	}
	//Se copia la parte 3
	for (i = g1_cuts_p[0] +  (g2_cuts_p[1] - g2_cuts_p[0]);
			i < g1->program.height; i++){
		new_list[i] = g2->program.list_inst[i];
	}
	program.list_inst = new_list;
	program.height = new_len;
	set_altered();

}
void Individual::crossover(Individual genome1, Individual genome2, Individual sister, Individual brother) {
	Individual *mom, *dad;
	genome1.check_max_min_instructions("genome1", "Antes Crossover");
	genome2.check_max_min_instructions("genome2", "Antes Crossover");
	int cuts_points_mom [2]= {0,0};
	int cuts_points_dad [2]= {0,0};
	/*Se clonan genome1 y 2 en mom y dad respectivamente.
	 * se puede direccionar nomas tambièn...
	 * ya que mom y dad son punteros
	 */
	select_mom_dad(genome1, genome2, mom, dad);
	try{
		int mom_segment_size = randint(1, mom->program.height - 1); //al menos 1 instruccion menos la penultima.
		int max_padding_mom = mom->program.height -mom_segment_size - 1;

		//desde donde se puede comenzar para que alcancen las instrucciones del segmento a cruzar
		cuts_points_mom[0] = randint(0, max_padding_mom);
		cuts_points_mom[1] = cuts_points_mom[0] + mom_segment_size;

		//el máximo segmento a cruzar es lo que le falta al mom para completar el máximo número de instrucciones permitidas
		int max_segment_full_mom = NUM_MAX_INSTRUCTIONS - (mom->program.height - mom_segment_size);
		int max_segment_num_min_in_dad = (dad->program.height + mom_segment_size) - NUM_MIN_INSTRUCTIONS;

		//se elije el menor de los máximos
        int max_segment_size_dad = max_segment_full_mom < max_segment_num_min_in_dad ? max_segment_full_mom : max_segment_num_min_in_dad;

        //si el maximo es mayor a la longitud del padre, se elige la longitud como máximo
        max_segment_size_dad = (dad->program.height - 1) < max_segment_size_dad ? (dad->program.height - 1) : max_segment_size_dad;

        //lo que hay que quitarle para que quede en máximo número de instrucciones
        int min_segment_full_dad = (dad->program.height + mom_segment_size) - NUM_MAX_INSTRUCTIONS;

        //lo que falta para completar el mínimo número de instrucciones al mom
        int min_segment_size_num_min_in_mom = NUM_MIN_INSTRUCTIONS - (mom->program.height - mom_segment_size);

        int min_segment_size_dad = (min_segment_full_dad > min_segment_size_num_min_in_mom) ? min_segment_full_dad : min_segment_size_num_min_in_mom;
        min_segment_size_dad = min_segment_size_dad <= 0 ? 1 :min_segment_size_dad;

        int dad_segment_size = randint(min_segment_size_dad, max_segment_size_dad);

        int max_padding_dad = dad->program.height - dad_segment_size - 1;

        //desde donde se puede comenzar para que alcancen las instrucciones del segmento a cruzar
        cuts_points_dad[0] = randint(0, max_padding_dad);
        cuts_points_dad[1] = cuts_points_dad[0] + dad_segment_size;

        //Se clonan los individuos
        sister = *mom->clone();
        brother = *dad->clone();
        //Se intercambian los bloques
        sister.exchange(mom, dad, cuts_points_mom, cuts_points_dad);
        brother.exchange(dad, mom, cuts_points_dad, cuts_points_mom);
        //Se modifica el indice
        sister.index = genome1.index;
        brother.index = genome2.index;

        //Checkeo de no inconsistencia
        sister.check_max_min_instructions("sister", "Despues Crossover");
        brother.check_max_min_instructions("brother", "Despues Crossover");
	}catch (exception e) {
		std::cout << "Error en Crossover";
	}
}
/* TODO: FALTAN LOS SIGUIENTES METODOS
 * def macro_mutation(genome):
 * def micro_mutation(genome):
 * def select_micro_mutacion_type(prob):
 * */

/*


Individual::
def get_effective_instructions(self):
        """
        intructions[i][0] = identificador de instucción
        intructions[i][1] = registro destino
        intructions[i][2] = registro operando 1
        intructions[i][3] = registro operando 2
        """
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
        """
        intructions[i][0] = identificador de instucción
        intructions[i][1] = registro destino
        intructions[i][2] = registro operando 1
        intructions[i][3] = registro operando 2
        """
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
        """
        Función de evaluación de fitnes
        """
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
            """
            Si ocurre una excepción el fitness se iguala a cero (Overflow muy probablemente)
            """
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
        """
        Función de evaluación, dado un objeto con el atributo config_position
        retorna una lista del error cuadrático
        para cada línea del los casos de evaluación
        """

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
        """ Return a string representation of Genome """
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
