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
	f << "nt\t" << NT << "\n";
	f << "delta\t" << DELTA << "\n";

	f << "\n----Muestra de datos\n";
	f << "lines\t" << LINES << "\n";
	f << "training_lines\t" << TRAINING_LINES << "\n";
	f << "validation_lines\t" << VALIDATION_LINES << "\n";
	f << "FILE_NAME\t" << FILE_NAME << "\n";


	f << "\n----Nuevos\n";
	f << "CONFIG_POSITION\t" << CONFIG_POSITION << "\n";
	f << "CNT_PRUEBAS\t" << CNT_PRUEBAS << "\n";
	f << "ERROR_TO_STOP\t" << ERROR_TO_STOP << "\n";
	f << "NUM_MAX_GENERATION\t" << NUM_MAX_GENERATION << "\n";
	f << "CANT_ESTANCAMIENTO\t" << CANT_ESTANCAMIENTO  << "\n";
	f << "ERROR_STEP\t" << ERROR_STEP << "\n";
	f << "MIN_ERROR_STEP\t" << MIN_ERROR_STEP << "\n";

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
	f << "P_REG_CONST_DELTA\t" << P_REG_CONST_DELTA << "\n";
	f << "P_DIVERSITY\t" << P_DIVERSITY << "\n";


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

void save_to_analisis_file(std::string folder, double err, double max, std::string faltante){
	std::ofstream f;
	std::string file = FILE_ANALISIS;
	f.open(file.c_str(), std::ofstream::app);
	f.precision(6);
	f<<folder<<";"<<err<<";"<<max<<";"<<faltante<<"\n";
	f.close();
}

void save_global_results(Individual * best_global , int cant_trafos, std::string folder) {
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
	std::vector<double *> error_list(cant_trafos);
	for (int j = 0; j < cant_trafos ; j++) { //Por cada transformador
		error_list[j] = best_global[j].eval_individual(VALIDATION);
	}
	double sum_prom_errores = 0;
	int size = VALIDATION_LINES + 3;
	f<<folder<<"\n";
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
	std::stringstream faltante;

	for (unsigned i = 0; i < error_list.size(); i++) {
		faltante<<error_list.at(i)[size-1]<<"#";
		sum_prom_errores += error_list.at(i)[size-2]; //el promedio de errores de la instancia de prueba
		if (error_list.at(i)[size-2] > max){ //El maximo error de los transformadores
			max = error_list.at(i)[size-2];
		}
	}
	double errprom = sum_prom_errores/(cant_trafos);
	f << "Error promedio total: ;" << errprom<< ";\n";
	f << "Error maximo: ;" << max << ";\n";
	f.close();
	save_to_analisis_file(folder, errprom, max, faltante.str());
}

void save_global_results_validation(Individual * best_global , int cant_trafos, std::string folder) {
	/*
	 * Se debe calcular el error promedio y el error maximo.
	 * Se debe escribir lo siguiente (agregando al archivo)
	 *
	 */
	std::ofstream f;
	std::string file = folder + "APROXIMACIONES_VALIDACION.csv";
	f.open(file.c_str());
	f.precision(6);
	std::string row;
	std::vector<double *> error_list(cant_trafos);
	for (int j = 0; j < cant_trafos ; j++) { //Por cada transformador
		error_list[j] = best_global[j].eval_individual_result(VALIDATION);
	}
	double sum_prom_errores = 0;
	int size = VALIDATION_LINES + 3;
	f<<folder<<"\n";
	for (int t = 0; t < size; t++) {
		if (t == size - 3) {
			f<<"---;";
		} else if (t == size - 2) {
			f<<"---;";
		} else if (t == size - 1) {
			f<<"---;";
		} else {
			f<<t<<";";
		}

		for (unsigned i = 0; i < error_list.size(); i++) {
			f << "" << std::fixed << error_list.at(i)[t] << ";";
		}
		f << "\n";
	}
	double max = error_list.at(0)[size-2];
	std::stringstream faltante;

	for (unsigned i = 0; i < error_list.size(); i++) {
		faltante<<error_list.at(i)[size-1]<<"#";
		sum_prom_errores += error_list.at(i)[size-2]; //el promedio de errores de la instancia de prueba
		if (error_list.at(i)[size-2] > max){ //El maximo error de los transformadores
			max = error_list.at(i)[size-2];
		}
	}
	double errprom = sum_prom_errores/(cant_trafos);
	f << "---;" << errprom<< ";\n";
	f << "---;" << max << ";\n";
	f.close();
}

int get_counters_sum(std::vector<int> vect){
	int total = 0;
	for (int i = 0; i < vect.size(); i++)
		total += vect[i];
	return total;
}

void trafo_counters_to_file(std::string trafo, std::string folder, int primero, Individual *best_init, Individual *best_end){
	std::ofstream f;
	std::string file = FILE_PROBABILIDADES;
	f.open(file.c_str(), std::ofstream::app);
	f.precision(6);
	if (primero){
		f << folder << ";"<<"Transf.;"<<"Migracion;"<<"Crossover;"<<"Macro_Mut;";
		f << "Macro_del;" << "Macro_ins;" << "Micro ;" << "Micro Operadores;" << "Micro Registros;";
		f << "Micro Constante;"<< "Err. Best Init;"<< "Err. Best End;" << "\n";
		f << "Parametrizado" << ";";
		f << P_MIGRATION << ";";
		f << P_CROSSOVER << ";";
		f << P_MACRO_MUTATION << ";";
		f << P_DEL << ";";
		f << P_INS<< ";";
		f << P_MICRO_MUTATION << ";";
		f << P_OPERMUT<< ";";
		f << P_REGMUT<< ";";
		f << P_CONSTMUT<< ";";
		f << "\n";

	}
	f << ";";
	f << "Transformador "<<trafo << " ;";
	f << get_counters_sum(Lgp::cant_migracion)				<< ";";
	f << get_counters_sum(Individual::cant_crossover)		<< ";";

	f << get_counters_sum(Individual::cant_macro)			<< ";";
	f << get_counters_sum(Individual::cant_macro_del)		<< ";";
	f << get_counters_sum(Individual::cant_macro_ins)		<< ";";

	f << get_counters_sum(Individual::cant_micro)			<< ";";
	f << get_counters_sum(Individual::cant_micro_ope)		<< ";";
	f << get_counters_sum(Individual::cant_micro_reg)		<< ";";
	f << get_counters_sum(Individual::cant_micro_const)		<< ";";
	f << best_init->error << ";";
	f << best_end->error << ";" << "\n";

	/* ************************************************************** */
	int veces = (NUM_GENERATIONS + 1) * GEN_TO_MIGRATE * DEMES;
	f<<";";
	f<<"Prob. Efect. Trafo #" << trafo << ":"<<";";
	f << ((double)get_counters_sum(Lgp::cant_migracion)/((NUM_GENERATIONS + 1) * DEMES))<< ";";
	f << ((double)get_counters_sum(Individual::cant_crossover)/veces)		<< ";";

	f << ((double)get_counters_sum(Individual::cant_macro)/(veces*2))			<< ";";
	f << ((double)get_counters_sum(Individual::cant_macro_del)/(double)get_counters_sum(Individual::cant_macro))		<< ";";
	f << ((double)get_counters_sum(Individual::cant_macro_ins)/(double)get_counters_sum(Individual::cant_macro))		<< ";";

	f << ((double)get_counters_sum(Individual::cant_micro)/(veces*2))			<< ";";
	f << ((double)get_counters_sum(Individual::cant_micro_ope)/(double)get_counters_sum(Individual::cant_micro))		<< ";";
	f << ((double)get_counters_sum(Individual::cant_micro_reg)/(double)get_counters_sum(Individual::cant_micro))		<< ";";
	f << (double)(get_counters_sum(Individual::cant_micro_const)/(double)get_counters_sum(Individual::cant_micro))		<< ";\n";
	f.close();
}
void global_counters_to_file(std::string folder, int global_cant_crossover, int global_cant_migracion,
		int global_cant_macro, int  global_cant_macro_del, int global_cant_macro_ins, int global_cant_micro,
		int global_cant_micro_reg,int global_cant_micro_const, int global_cant_micro_ope, int cant_trafos)
{
	std::ofstream f;
	std::string file = FILE_PROBABILIDADES;
	f.open(file.c_str(), std::ofstream::app);
	f.precision(6);

	f << ";";
	f << "TOTAL: ;";
	f << global_cant_migracion << ";";
	f << global_cant_crossover << ";";


	f << global_cant_macro << ";";
	f << global_cant_macro_del << ";";
	f << global_cant_macro_ins << ";";

	f << global_cant_micro << ";";
	f << global_cant_micro_ope << ";";
	f << global_cant_micro_reg << ";";
	f << global_cant_micro_const << ";\n";

	/* ************************************ */
	double veces =  (NUM_GENERATIONS + 1) * GEN_TO_MIGRATE * cant_trafos * DEMES;
	f << ";";
	f << "PROBABILIDAD: ;";
	f << (double)global_cant_migracion/((NUM_GENERATIONS + 1) * DEMES * cant_trafos)<< ";";
	f << (double)global_cant_crossover/veces << ";";

	f << ((double)global_cant_macro/(veces*2)) << ";";
	f << ((double)global_cant_macro_del/global_cant_macro) << ";";
	f << ((double)global_cant_macro_ins/global_cant_macro) << ";";

	f << ((double)global_cant_micro/(veces*2)) << ";";
	f << ((double)global_cant_micro_ope/global_cant_micro) << ";";
	f << ((double)global_cant_micro_reg/global_cant_micro) << ";";
	f << ((double)global_cant_micro_const/global_cant_micro) << ";\n";
	/* ************************************ */
	f << ";";
	f << "GENERACIONES: ;";
	f << NUM_GENERATIONS << ";\n";
	/* ************************************ */
	f << ";";
	f << "GEN_TO_MIGRATE: ;";
	f << GEN_TO_MIGRATE << ";\n";
	/* ************************************ */
	f << ";";
	f << "DEMES: ;";
	f << DEMES << ";\n";
	f.close();
}

void errors_generation_to_file(std::string trafo, std::string folder, int tipo){
	std::ofstream f;
	std::string file="";
	std::vector<double *> lista;

	if (tipo==TRAINING){
		file = folder + FILE_NAME_ERRORES_X_DEME_TRAINING;
		lista = Lgp::errores_x_deme_training;
	}else{
		file = folder + FILE_NAME_ERRORES_X_DEME_VALIDATION;
		lista = Lgp::errores_x_deme_validation;
	}


	f.open(file.c_str(), std::ofstream::app);
	f.precision(6);
//	f << folder << "\n";
	f << "Generacion,";
	for (int i=0; i<DEMES; i++)
		f<< "DEME" << i << ",";
	f<< "\n";
	for (int i = 0; i < Lgp::generacion_real; i++){//Por cada generacion
		f<< i << "," ;
		for (int j = 0; j < lista.size(); j++){//Por cada deme
			f<< lista[j][i] << ","; //Impresion de los errores del deme i...
		}
		f<< "\n";
	}
	f.close();
}


