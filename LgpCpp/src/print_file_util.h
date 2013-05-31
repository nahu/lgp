/*
 * print_file_util.h
 *
 *  Created on: April 25, 2013
 *      Author: Vanessa Cañete, Nahuel Hernández
 */

void parameters_to_file(std::string f_param) {
	std::ofstream f;
	f.open(f_param.c_str());
	f << "PARÁMETROS\n";
	f << "----------------------------------------------------------------\n";
	f << "--Problema\n";
	f << "config\t" << CONFIG << "\n";
	f << "n\t" << N << "\n";
	f << "k\t" << K << "\n";

	f << "\n----Muestra de datos\n";
	f << "lines\t" << LINES << "\n";
	f << "training_lines\t" << TRAINING_LINES << "\n";
	f << "validation_lines\t" << VALIDATION_LINES << "\n";

	f << "\n--Algoritmo Evolutivo\n";
	f << "num_generations\t" << NUM_GENERATIONS << "\n";
	f << "pool_size\t" << POOL_SIZE << "\n";
	f << "population_size\t" << POPULATION_SIZE << "\n";
	f << "demes\t" << DEMES << "\n";
	f << "migration_rate\t" << MIGRATION_RATE << "\n";
	f << "gen_to_migrate\t" << GEN_TO_MIGRATE << "\n";
	f << "w_ob1\t" << W_OB1 << "\n";
	f << "w_ob2\t" << W_OB2 << "\n";

	f << "\n--Probabilidades\n";
	f << "p_migration\t" << P_MIGRATION << "\n";
	f << "p_migration_criteria\t" << P_MIGRATION_CRITERIA << "\n";
	f << "--Registros constantes\n";
	f << "p_reg_op1_const\t" << P_REG_OP1_CONST << "\n";
	f << "p_reg_op2_const\t" << P_REG_OP2_CONST << "\n";
	f << "p_const_in\t" << P_CONST_IN << "\n";

	f << "\n----Crossover\n";
	f << "p_crossover\t" << P_CROSSOVER << "\n";

	f << "\n----Mutación Macro\n";
	f << "p_macro_mutation\t" << P_MACRO_MUTATION << "\n";
	f << "p_del\t" << P_DEL << "\n";
	f << "p_ins\t" << P_INS << "\n";

	f << "\n----Mutación Micro\n";
	f << "p_micro_mutation\t" << P_MICRO_MUTATION << "\n";
	f << "p_opermut\t" << P_OPERMUT << "\n";
	f << "p_regmut\t" << P_REGMUT << "\n";
	f << "p_constmut\t" << P_CONSTMUT << "\n";

	f << "\n--Registros\n";
	f << "\n----Cantidades\n";
	f << "num_registers\t" << NUM_REGISTERS << "\n";
	f << "   num_out_registers\t" << NUM_OUT_REGISTERS << "\n";
	f << "num_var_register\t" << NUM_VAR_REGISTER << "\n";
	f << "num_conts_registers\t" << NUM_CONTS_REGISTERS << "\n";
	f << "   num_const_random_registers\t" << NUM_CONST_RANDOM_REGISTERS
			<< "\n";
	f << "   num_const_in_registers\t" << NUM_CONST_IN_REGISTERS << "\n";

	f << "\n--Valores en vector instrucción\n";
	f << "\n----Operaciones\n";
	f << "num_operators\t" << NUM_OPERATORS << "\n";
	f << "op_min\t" << OP_MIN << "\n";
	f << "op_max\t" << OP_MAX << "\n";

	f << "\n----Salida\n";
	f << "reg_out\t" << REG_OUT << "\n";

	f << "\n----Variables\n";
	f << "var_min\t" << VAR_MIN << "\n";
	f << "var_max\t" << VAR_MAX << "\n";

	f << "\n----Constantes aleatorias\n";
	f << "cons_al_min\t" << CONST_AL_MIN << "\n";
	f << "cons_al_max\t" << CONST_AL_MAX << "\n";

	f << "\n------Valores\n";
	f << "const_min\t" << CONST_MIN << "\n";
	f << "const_max\t" << CONST_MAX << "\n";
	f << "step_size_const\t" << STEP_SIZE_CONST << "\n";

	f << "\n----Constantes de entrada\n";
	f << "cons_in_min\t" << CONST_IN_MIN << "\n";
	f << "cons_in_max\t" << CONST_IN_MAX << "\n";

	f << "\n--Instrucciones\n";
	f << "num_ini_instructions\t" << NUM_INI_INSTRUCTIONS << "\n";
	f << "num_max_instructions\t" << NUM_MAX_INSTRUCTIONS << "\n";
	f << "num_min_instructions\t" << NUM_MIN_INSTRUCTIONS << "\n";

	f << "\n--Procesos\n";
	f << "chunk_size\t" << CHUNK_SIZE << "\n";
	f << "num_processors\t" << NUM_PROCESSORS << "\n";

	f.close();
}

void errors_to_file(std::string file, std::vector<double *> error_list, int size) {
	/*
	 * Se recibe una lista que contiene por cada "mejor individio" una lista de errores
	 * por muestra. Entonces, se debe imprimir en columnas los errores por individuo.
	 * Las ultimas 3 posiciones de cada lista tiene: sumatoria de errores, promedio, y transf.
	 */
	std::ofstream f;
	f.open(file.c_str());
	f.precision(6);
	std::string row;
	for (int t = 0; t < size; t++) {
		if (t == size - 3) {
			f<<"Error total;";
		} else if (t == size - 2) {
			f<<"Error promedio;";
		} else if (t == size - 1) {
			f<<"Posicion;";
		} else {
			f<<t<<";";
		}

		for (unsigned i = 0; i < error_list.size(); i++) {
			f << "" << std::fixed << error_list.at(i)[t] << ";";
		}
		f << "\n";
	}
	f.close();
}

void programs_to_file (std::string file, Individual ** best_individuals) {
	std::ofstream f;
	f.open(file.c_str());
	f.precision(6);
	for (int i = 0; i < DEMES; i++) {
		f << "*********************************************************************************"<< std::endl;
		f << "Best Individual: "<< i << std::endl;
		f << "*********************************************************************************"<< std::endl;
		f << "Config Pos: " << best_individuals[i]->config_position << "\n";
		f << "Training error: " << best_individuals[i]->error << "\n";
		f << "Trainig Deviation: " << best_individuals[i]->sigma << "\n";
		f << "Fitness: " << best_individuals[i]->fitness << "\n";
		f << "List Size: " << best_individuals[i]->program->height << "\n";
		f << "List effective Size: " << best_individuals[i]->program->n_eff << "\n";

		f << "-----------------------"<< std::endl;
		f << "Lista de instrucciones: "<< std::endl;
		f << "-----------------------"<< std::endl;

		for (int j = 0; j<best_individuals[i]->program->height ; j++){
			f << best_individuals[i]->program->list_inst[j].get_str_instruction();
		}

		f << "------------------------"<< std::endl;
		f << "Instrucciones efectivas: "<< std::endl;
		f << "------------------------"<< std::endl;

		for (int j = 0; j<best_individuals[i]->program->n_eff ; j++){
			f << best_individuals[i]->program->effective_list_inst[j].get_str_instruction();
		}

		f << "-----------"<< std::endl;
		f << "Registros: "<< std::endl;
		f << "-----------"<< std::endl;
		for (int j = 0; j < NUM_INDIVIDUAL_REGISTERS; j++) {
			f << "r_all[" << j << "] = " << best_individuals[i]->program->list_reg[j]<< "\n";
		}
	}
	f.close();
}

void save_global_results(Individual * best_global , std::string folder) {
	/*
	 * Se debe calcular el error promedio y el error maximo.
	 * Se debe escribir lo siguiente (agregando al archivo)
	 *
	 */
	std::ofstream f;
	std::string file = folder + "RESUMEN_VALIDACION.csv";
	f.open(file.c_str());
	f.precision(6);
	std::string row;
	std::vector<double *> error_list(N-K);
	for (int j = 0; j < N-K; j++) {
		error_list[j] = best_global[j].eval_individual(VALIDATION);
	}
	double sum_prom_errores = 0;
	int size = VALIDATION_LINES + 3;

	for (int t = 0; t < size; t++) {
		if (t == size - 3) {
			f<<"Error total;";
		} else if (t == size - 2) {
			f<<"Error promedio;";
		} else if (t == size - 1) {
			f<<"Posicion;";
		} else {
			f<<t<<";";
		}

		for (unsigned i = 0; i < error_list.size(); i++) {
			f << "" << std::fixed << error_list.at(i)[t] << ";";
		}
		f << "\n";
	}
	double max = error_list.at(0)[size-2];
	for (unsigned i = 0; i < error_list.size(); i++) {
		sum_prom_errores += error_list.at(i)[size-2]; //el promedio de errores de la instancia de prueba
		if (error_list.at(i)[size-2] > max){ //El maximo error de los transformadores
			max = error_list.at(i)[size-2];
		}
	}
	f << "Error promedio total: ;" << sum_prom_errores/(N-K)<< ";\n";
	f << "Error maximo: ;" << max << ";\n";
	f.close();
}

