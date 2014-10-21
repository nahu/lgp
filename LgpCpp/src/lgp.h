/*
 * lgp.h
 *
 *  Created on: Mar 16, 2013
 *      Author: Vanessa Cañete, Nahuel Hernández
 */

//toda la parte de LGPMAIN.py que están dentro del la clase lgpmain y tiene el algoritmo evolutivo en sí

class Lgp {
public:
	Lgp(int config_position, int demes, int population_size, int num_generations);
	~Lgp();
	Individual * best_individual_in_training(bool verbose=false);
	Individual ** best_individuals_of_demes(int tipo);
	bool termination_criteria();
	void deme_evolve(int deme_index, int current_gen);
	void evolve();

	unsigned int generation;
	unsigned int num_generation;
	//int config_position;
	double best_error;
	int num_demes;
	Deme * population;
	static std::vector<int>  cant_migracion;
	static std::vector<double *> errores_x_deme_training;
	static std::vector<double *> errores_x_deme_validation;
	static int generacion_real;
};
/*
 * Inicializacion de variables (para que no de "undefined reference to"
 */
std::vector<int>  Lgp::cant_migracion (NUM_PROCESSORS,0);
std::vector<double *> Lgp::errores_x_deme_training (DEMES);
std::vector<double *> Lgp::errores_x_deme_validation (DEMES);
int Lgp::generacion_real = 0;


Lgp::Lgp(int config_position, int demes, int population_size, int num_generation) {
	//std::cout<<"Const. LGP\n";

	generation = 0;
	this->generacion_real = 0;
	best_error = HUGE_NUMBER;
	this->num_generation = num_generation;
	int div, pop_size_per_deme;

	div = population_size / demes;

	//std::cout<<"div"<<div<<"\n";

	pop_size_per_deme = div;
	//pop_size_per_deme = 200;

	if (div < 0) {
		pop_size_per_deme = 1;
	}

	num_demes = demes;
	population = new Deme[num_demes];

	//se podría paralelizar, create_new_deme también.. ver si es factible hacerlo en ambos lugares
	for (int i = 0; i < num_demes; i++) {
		population[i].create_new_deme(pop_size_per_deme, config_position);
		errores_x_deme_training[i] = new double[NUM_MAX_GENERATION*GEN_TO_MIGRATE + GEN_TO_MIGRATE];
		errores_x_deme_validation[i] = new double[NUM_MAX_GENERATION*GEN_TO_MIGRATE + GEN_TO_MIGRATE];
	}
	std::cout<<"Inicializado con tamaño "<< NUM_MAX_GENERATION*GEN_TO_MIGRATE + GEN_TO_MIGRATE<< std::endl;
}


Lgp::~Lgp() {
	delete [] population;
}


Individual* Lgp::best_individual_in_training(bool verbose) {
	Individual ** deme_best = new Individual *[num_demes];
	Individual * best = 0;

	//todo: paralelizar
	int chunks = num_demes / (NUM_PROCESSORS);
	#ifdef PARALLELIZED
	#pragma omp parallel for schedule(static, chunks)
	#endif
	for (int i = 0; i < num_demes; i++) {
		deme_best[i] = population[i].best_training();
	}

	best = deme_best[0];

	for (int i = 1; i < num_demes; i++) {
		if (Individual::compare_fitness(*best, *deme_best[i])) {
			best = deme_best[i];
		}
	}

	if (verbose) {
		std::cout << "Los mejores de Entrenamietno\n";
		for (int i = 0; i < num_demes; i++) {
			std::cout << "+++++++++DEME " << i << "\n";
			deme_best[i]->print_individual();
		}
	}

	best_error = best->error;

	delete [] deme_best;
	return best;
}


Individual** Lgp::best_individuals_of_demes(int tipo) {
	Individual** demes_best = new Individual*[num_demes];

	//todo: paralelizar
	if (tipo == TRAINING) {
		for (int i = 0; i < num_demes; i++) {
			demes_best[i] = population[i].best_training();
		}
	} else if (tipo == VALIDATION) {
		for (int i = 0; i < num_demes; i++) {
			demes_best[i] = population[i].best_validation();
		}
	}

	return demes_best;
}


inline bool Lgp::termination_criteria() {

	//if ((best_error < ERROR_TO_STOP) || (generation > NUM_MAX_GENERATION)) {
	if (generation > NUM_MAX_GENERATION) {
		return true;
	} else {
		return false;
	}
}

int get_win_pos(Participant * indices, int ini, int end) {
	int pos = -1;
	for ( ;ini < end; ++ini) {
		if (indices[ini].status == WINNER) {
			pos = indices[ini].pop_position;
		}
	}
	return pos;
}


//se podría llevar a deme
void Lgp::deme_evolve(int deme_index, int current_gen) {
	//std::cout << "deme evolve " << std::endl;
	Participant * selected_indices;
	//Individual ** to_tournament[2];
	Individual * winners[TOURNAMENTS];

	int ini[TOURNAMENTS], end[TOURNAMENTS];
	int tournaments_selected[2];

	Individual ** winners_selected[2];

	int diver = 0;
	for (int gen = 0; gen < GEN_TO_MIGRATE; gen++) {
		//std::cout << ".....GEN to migrate" << gen << "\n";
		selected_indices = population[deme_index].indices_selection(POOL_SIZE * TOURNAMENTS);

		int limite = 0;
		for (int t = 0; t < TOURNAMENTS; t++) {
			ini[t] = limite;
			end[t] = limite + POOL_SIZE;
			limite += POOL_SIZE;
		}

//		std::cout << "%%%%%% Ganadores de torneos" << "\n";


		for (int i = 0; i < TOURNAMENTS; i++) {
			winners[i] = population[deme_index].tournament(selected_indices, ini[i], end[i]);
//			std::cout << "Winner " << i << "\n";
//			winners[i]->print_individual(true);
//			std::cout << "\n";
		}

		//guardar los dos ganadores por diversidad o por fitness en winners_mutated
		if (random_flip_coin(P_DIVERSITY)) {
//			std::cout << "++++por diversidad" << "\n";
			//encontrar los dos mas distantes
			diver = 1;
			int max_distance = -1;
			int distance;

			//establecer la diversidad de los ganadores
			for (int i = 0; i < TOURNAMENTS; i++) {
//				std::cout << "winner " << i << "\n";
				winners[i]->program->set_op_diversity();
//				if (winners[i]->check_op_diversity()) {
//					winners[i]->print_op_diversity();
//				}
			}

			//calcular las distancias y encontrar los dos mas alejados
			for (int i = 0; i < TOURNAMENTS; i++) {
				for (int j = i + 1; j < TOURNAMENTS - 1; j++) {
					distance = winners[i]->manhattan_op_distance(winners[j]);
//					std::cout << " === distincia " << distance << "\n";
					if (distance > max_distance) {
						max_distance = distance;
						tournaments_selected[0] = i;
						tournaments_selected[1] = j;
					}
				}
			}

//			std::cout << "----torneo d 1: " << tournaments_selected[0] << "\n";
//			std::cout << "----torneo d 2: " << tournaments_selected[1] << "\n";

		} else {
//			std::cout << "----por fitness" << "\n";
			//seleccionar los dos con mayor fitness
			tournaments_selected[0] = 0;

			for (int i = 1; i < TOURNAMENTS; i++) {
				if (Individual::compare_fitness(*winners[tournaments_selected[0]], *winners[i])) {
					tournaments_selected[0] = i;
				}
			}

			tournaments_selected[1] = 0;
//			std::cout << "**** torneo 0: " << tournaments_selected[0] << "\n";
			while (tournaments_selected[1] == tournaments_selected[0]) {
				tournaments_selected[1]++;
//				std::cout << "    **** torneo 1: " << tournaments_selected[1] << "\n";
			}

			for (int i = 0; i < TOURNAMENTS; i++) {
				if (i != tournaments_selected[0]) {
					if (Individual::compare_fitness(*winners[tournaments_selected[1]], *winners[i])) {
						tournaments_selected[1] = i;
					}
				}
			}
//			std::cout << "----torneo f 1: " << tournaments_selected[0] << "\n";
//			std::cout << "----torneo f 2: " << tournaments_selected[1] << "\n";
		}

		for (int i = 0; i < 2; i++) {
			Individual ** win_and_offpring = new Individual*[2];

			win_and_offpring[0] = new Individual(*winners[tournaments_selected[i]]);
			win_and_offpring[1] = winners[tournaments_selected[i]];




			winners_selected[i] = win_and_offpring;

//			std::cout << "\n$$$$$$ Ganadores para modificar " << i << "\n\n";
//			winners_selected[i][0]->print_individual();
//			std::cout << "\n";
//			winners_selected[i][1]->print_individual();

			if (random_flip_coin(P_MACRO_MUTATION)) {
				winners_selected[i][0]->macro_mutation();
			}

			if (random_flip_coin(P_MICRO_MUTATION)) {
				winners_selected[i][0]->micro_mutation();
			}
		}

		if (random_flip_coin (P_CROSSOVER)) {
			//los que pudieron ser modificados por la macro y micro mutación
			Individual::crossover(winners_selected[0][0], winners_selected[1][0]);
		}


		for (int j = 0; j < (TOURNAMENTS / 2); j++) {
			population[deme_index].override_loosers(selected_indices, ini[j], end[j], winners_selected[0]);
		}

		for (int j = (TOURNAMENTS / 2); j < TOURNAMENTS; j ++) {
			population[deme_index].override_loosers(selected_indices, ini[j], end[j], winners_selected[1]);
		}

		for (int i = 0; i < 2; i++) {
			//eliminar la copia del ganador de cada torneo, se creó en mas arriba
			delete winners_selected[i][0];
			delete [] winners_selected[i];
		}

		delete [] selected_indices;

		/* Almacenamiento de errores por deme */
		Lgp::errores_x_deme_training[deme_index][current_gen+gen] =  population[deme_index].best_training()->error;
		Lgp::errores_x_deme_validation[deme_index][current_gen+gen] =  population[deme_index].best_validation()->error;
	}

	population[deme_index].evaluate_individuals();

	//int index = 0;
	/*
	std::cout<<"antes del sort\n";


	for (std::vector<Individual>::iterator it = population[deme_index].list_ind->begin(); it != population[deme_index].list_ind->end(); ++it) {

		(*it).check(deme_index, index);
		index++;
	}
	*/

	if (random_flip_coin(P_MIGRATION_CRITERIA)) {
		std::sort(population[deme_index].list_ind->begin(), population[deme_index].list_ind->end(), compare_ob1());

	} else {
		std::sort(population[deme_index].list_ind->begin(), population[deme_index].list_ind->end(), compare_ob2());
	}



/*

	if (deme_index == 0) {
		int index = 0;
		std::cout<<"Despues del sort\n";
	for (std::vector<Individual>::iterator it = population[deme_index].list_ind->begin(); it != population[deme_index].list_ind->end(); ++it) {
		//if (index % 4 == 0)
			(*it).check(deme_index, index);

		index++;
	}
	}
*/

}



void Lgp::evolve() {
	std::cout<<"evolve\n";
	//Inicializa los contadores de operadores.
	//Todo: guardar el fitnes del el fitnes del mejor en la primera generación 	y del mejor en la última generación
	int for_replace;
	std::vector<Individual>::iterator ini, end, it;

	double actual_diff = 0.0;
	double sum_diff = 0.0;
	int stopped_gen = 0;
	double best_init, best_end;
	int re_creation_times = 0;

	while (!termination_criteria()) {
		generation++;

		std::cout << "Generación #" << generation << "Generacion REAL "<< Lgp::generacion_real << "\n";
		for_replace = MIGRATION_RATE * (float) population[0].deme_size;
		//for_replace = 3;
		//std::cout << "for replace " << for_replace << "\n";

		best_individual_in_training();
		best_init = best_error;

		#ifdef PARALLELIZED
		int chunks = num_demes / (NUM_PROCESSORS);
		#pragma omp parallel for schedule(static, chunks)
		#endif
		for (int i = 0; i < num_demes; i++) {
			deme_evolve(i, Lgp::generacion_real);

			//int index = 0;
/*			for (std::vector<Individual>::iterator it = population[i].list_ind->begin(); it != population[i].list_ind->end(); ++it) {
				(*it).check(i, index);
				index++;
			}*/
		}

		Lgp::generacion_real += GEN_TO_MIGRATE;

		if (random_flip_coin(P_MIGRATION)) {
			#ifdef USING_OMP
			Lgp::cant_migracion[omp_get_thread_num()]+=1;
			#endif
			ini = population[num_demes - 1].list_ind->end() - for_replace;
			end = population[num_demes - 1].list_ind->end();
			it = population[0].list_ind->begin();

			int cont = 0;

			for ( ; ini != end; ++ini) {
				/*
				std::cout << "·························" << "\n";
				std::cout << "Este se borra" << "\n";
				(*ini).check(num_demes -1, cont);
				std::cout << "Se reemplaza por este" << "\n";
				(*it).check(0, cont);
				*/

				if (Individual::compare_fitness(*ini, *it)) {
					(*ini) = (*it);
				}

				it++;
				cont++;
			}

			//std::cout << "migrados: " << cont << "\n";
			if (cont != for_replace) {
				std::cout << "no se copio todo " << cont << "\n";
			}
		}

		for (int i = 1; i < num_demes; i++) {
			if (random_flip_coin (P_MIGRATION)) {
				#ifdef USING_OMP
				Lgp::cant_migracion[omp_get_thread_num()]+=1;
				#endif

				ini = population[i - 1].list_ind->end() - for_replace;
				end = population[i - 1].list_ind->end();
				it = population[i].list_ind->begin();

				int cont = 0;

				for ( ; ini != end; ++ini) {
					/*
					std::cout << "·························" << "\n";
					std::cout << "Este se borra" << "\n";
					(*ini).check(i  -1, cont);
					std::cout << "Se reemplaza por este" << "\n";
					(*it).check(i, cont);
					*/

					if (Individual::compare_fitness(*ini, *it)) {
						(*ini) = (*it);
					}

					it++;
					cont++;
				}
				//std::cout << "migrados: " << cont << "\n";
				if (cont != for_replace) {
					std::cout << "no se copio todo " << cont << "\n";
				}
			}
		}

		/* ******************** Controles para estancamiento ******************** */

		best_individual_in_training();
		best_end = best_error;
		actual_diff = (best_init - best_end);


		std::cout << "generación #" << generation;
		std::cout << "  - best_init: " << best_init;
		std::cout << "  - best_end: " << best_end;
		std::cout << "  - diff: " << actual_diff << "\n";


		/*
		// Si se mantuvo el error despues de GEN_TO_MIGRATE, o si el avance actual es menor al anterior
		if (stopped_gen < CANT_ESTANCAMIENTO) {
			sum_diff += actual_diff;
			stopped_gen++;
		} else { //stopped_gen == CANT_ESTANCAMIENTO
				// Si se mantuvo el error despues de GEN_TO_MIGRATE, o si el avance actual es menor al anterior
			std::cout << "Condicion\n";
			std::cout << "sum_dif: "<< sum_diff << " < ";
			std::cout << "ERROR_STEP/generation: "<< (ERROR_STEP/generation) << " || ";
			std::cout << "sum_dif: "<< sum_diff << " < ";
			std::cout << "MIN_ERROR_STEP: "<< MIN_ERROR_STEP << "\n";

			if (sum_diff < (ERROR_STEP/generation) || sum_diff < (MIN_ERROR_STEP)){
				std::cout << "ESTANCAMIENTO: Se procede a realizar las mutaciones a toda la poblacion \n";
				re_creation_times++;
				//todo paralelizar
				int chunks = num_demes / (NUM_PROCESSORS);
				#pragma omp parallel for schedule(static, chunks)
				for (int i = 0; i < num_demes; i++) {

					for (std::vector<Individual>::iterator j = population[i].list_ind->begin(); j != population[i].list_ind->end(); ++j){
						switch(randint(1,3)) {
						case 1:
							(*j).macro_mutation();
							break;
						case 2:
							(*j).micro_mutation();
							break;
						case 3:

							Individual * new_individual = new Individual();
							Individual * temp = new Individual(*j);
							new_individual->create_new_individual((*j).config_position);


							Individual::crossover(new_individual, temp);


							(*j).operator=(*temp);


							delete new_individual;
							delete temp;

							break;
						}
					}

				}
			}
			sum_diff = 0.0;
			stopped_gen = 0;

		}
		*/
		/* ***************** Fin de Controles para estancamiento ***************** */

		if ((generation % FREQ_STATS) == 0 || generation == 1) {
			std::cout << "\n==================================================\n";
			std::cout << "Generación #" << generation << "\n";
			std::cout << "Veces recreadas: " << re_creation_times;
			std::cout << "\n==================================================\n";
			best_individual_in_training(true);
			re_creation_times = 0;
		}
	}
	best_individual_in_training();
}
