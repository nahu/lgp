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
	void deme_evolve(int deme_index);
	void evolve();

	unsigned int generation;
	unsigned int num_generation;
	//int config_position;
	double best_error;
	int num_demes;
	Deme * population;
	static std::vector<int>  cant_migracion;
};
/*
 * Inicializacion de variables (para que no de "undefined reference to"
 */
std::vector<int>  Lgp::cant_migracion (NUM_PROCESSORS,0);

Lgp::Lgp(int config_position, int demes, int population_size, int num_generation) {
	//std::cout<<"Const. LGP\n";

	generation = 0;
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
	}
}


Lgp::~Lgp() {
	delete [] population;
}


Individual* Lgp::best_individual_in_training(bool verbose) {
	Individual ** deme_best = new Individual *[num_demes];
	Individual * best = 0;

	//todo: paralelizar
	int chunks = num_demes / (NUM_PROCESSORS);
	#pragma omp parallel for schedule(static, chunks)
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

	if ((best_error < ERROR_TO_STOP) || (generation > NUM_MAX_GENERATION)) {
		return true;
	} else {
		return false;
	}
}


//se podría llevar a deme
void Lgp::deme_evolve(int deme_index) {
	Participant * selected_indices;
	//Individual ** to_tournament[2];
	Individual ** winners[2];
	int ini[2], end[2];


	for (int gen = 0; gen < GEN_TO_MIGRATE; gen++) {
		selected_indices = population[deme_index].indices_selection(POOL_SIZE * 2);

		ini[0] = 0;
		end[0] = POOL_SIZE;
		ini[1] = POOL_SIZE;
		end[1] = POOL_SIZE * 2;

		for (int i = 0; i < 2; i++) {
			winners[i] = population[deme_index].tournament_with_mutation(selected_indices, ini[i], end[i]);
		}

		if (random_flip_coin (P_CROSSOVER)) {
			//los que pudieron ser modificados por la macro y micro mutación
			Individual::crossover(winners[0][0], winners[1][0]);
		}

		for (int i = 0; i < 2; i++) {
			population[deme_index].override_loosers(selected_indices, ini[i], end[i], winners[i]);
			//eliminar la copia del ganador de cada torneo, se creó en tournament_with_mutation
			delete winners[i][0];
			delete [] winners[i];
		}

		delete [] selected_indices;
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
	//std::cout<<"evolve\n";
	//Inicializa los contadores de operadores.
	//Todo: guardar el fitnes del el fitnes del mejor en la primera generación 	y del mejor en la última generación
	int for_replace;
	std::vector<Individual>::iterator ini, end, it;

	double actual_diff = 0.0;
	int stopped_gen = 0;
	double best_init, best_end;
	int re_creation_times = 0;

	while (!termination_criteria()) {
		generation++;
		//std::cout << "Generación #" << generation << "\n";
		for_replace = MIGRATION_RATE * (float) population[0].deme_size;
		//for_replace = 3;
		//std::cout << "for replace " << for_replace << "\n";

		best_individual_in_training();
		best_init = best_error;

		//todo paralelizar
		int chunks = num_demes / (NUM_PROCESSORS);
		#pragma omp parallel for schedule(static, chunks)
		for (int i = 0; i < num_demes; i++) {
			deme_evolve(i);

			//int index = 0;
/*			for (std::vector<Individual>::iterator it = population[i].list_ind->begin(); it != population[i].list_ind->end(); ++it) {
				(*it).check(i, index);
				index++;
			}*/
		}

		if (random_flip_coin(P_MIGRATION)) {
			Lgp::cant_migracion[omp_get_thread_num()]+=1;
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
				Lgp::cant_migracion[omp_get_thread_num()]+=1;
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
		actual_diff = (best_end - best_init);


		std::cout << "generación #" << generation;
		std::cout << "  - best_init: " << best_init;
		std::cout << "  - best_end: " << best_end;
		std::cout << "  - diff: " << actual_diff << "\n";;

		/* Si se mantuvo el error despues de GEN_TO_MIGRATE, o si el avance actual es menor al anterior */
		if (actual_diff < (ERROR_STEP/generation) || actual_diff < (MIN_ERROR_STEP)){
			stopped_gen++;
		}

		if (stopped_gen == CANT_ESTANCAMIENTO) {
			std::cout << "ESTANCAMIENTO: Se procede a realizar las mutaciones a toda la poblacion \n";
			re_creation_times++;
			//todo paralelizar
			int chunks = num_demes / (NUM_PROCESSORS);
			#pragma omp parallel for schedule(static, chunks)
			for (int i = 1; i < num_demes; i++) {

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
						Individual * del_new_individual = new_individual;
						Individual * del_temp = temp;

						Individual::crossover(new_individual, temp);
						(*j) = (*new_individual);

						delete del_new_individual;
						delete del_temp;
						delete temp;
					}
				}

			}
			stopped_gen = 0;

		}
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
