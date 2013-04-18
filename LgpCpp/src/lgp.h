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
	Individual * best_individual_in_training();
	bool termination_criteria();
	void deme_evolve(int deme_index);
	void evolve();

	unsigned int generation;
	unsigned int num_generation;
	//int config_position;
	int num_demes;
	Deme * population;
};

Lgp::Lgp(int config_position, int demes, int population_size, int num_generation) {
	generation = 0;
	this->num_generation = num_generation;


	int div, pop_size_per_deme;
	div = population_size / demes;
	pop_size_per_deme = div ? div > 0 : 1;

	num_demes = demes;
	population = new Deme[num_demes];

	//se podría paralelizar, create_new_deme también.. ver si es factible hacerlo en ambos lugares
	for (int i = 0; i < num_demes; i++) {
		population[i].create_new_deme(pop_size_per_deme, config_position);
	}
}

Individual* Lgp::best_individual_in_training() {
	Individual ** deme_best = new Individual * [num_demes];
	Individual * best = 0;

	//paralelizar
	for (int i = 0; i < num_demes; i++) {
		deme_best[i] = population[i].best_training();
	}

	best = deme_best[0];

	for (int i = 1; i < num_demes; i++) {
		if (Individual::compare_fitness(*deme_best[i], *best)) {
			best = deme_best[i];
		}
	}

	std::cout << "Los mejores de Entrenamietno\n";
	for (int i = 0; i < num_demes; i++) {
		std::cout << "+++++++++DEME " << i << "\n";
		deme_best[i]->print_individual();
	}

	return best;
}


inline bool Lgp::termination_criteria() {
	return (generation > num_generation);
}

//se podría llevar a deme
void Lgp::deme_evolve(int deme_index) {
	std::vector<Participant> selected_indices;
	//Individual ** to_tournament[2];
	Individual ** winners[2];
	participant_iter ini[2], end[2];


	for (int gen = 0; gen < GEN_TO_MIGRATE; gen++) {
		selected_indices = population[deme_index].indices_selection(POOL_SIZE * 2);
		ini[0] = selected_indices.begin();
		end[0] = selected_indices.begin() + POOL_SIZE;
		ini[1] = selected_indices.begin() + POOL_SIZE + 1;
		end[1] = selected_indices.end();

		for (int i = 0; i < 2; i++) {
			winners[i] = population[deme_index].tournament_with_mutation(selected_indices, ini[i], end[i]);
		}

		if (random_flip_coin(P_CROSSOVER)) {
			;//Individual::crossover(winners[0][0], winners[1][0]);
		}

		/*
		if not sister.index == winners[0][0].index:
		winners[0][0] = brother
		winners[1][0] = sister
		*/


		for (int i = 0; i < 2; i++) {
			population[deme_index].override_loosers(selected_indices, ini[i], end[i], winners[i]);
			//eliminar la copia del ganador de cada torneo, se creó en tournament_with_mutation
			delete winners[i][0];
			delete [] winners[i];
		}
	}

	if (random_flip_coin(P_MIGRATION_CRITERIA)) {
		//(population.internal_pop).sort(cmp=Individual.compare_error_prom, reverse = True)
	} else {
		//(population.internal_pop).sort(cmp=Individual.compare_deviation_in_error, reverse = True)
	}

}


void Lgp::evolve() {
	int for_replace;
	int stats;
	std::vector<Individual>::iterator ini, end, it;

	while(!termination_criteria()) {
		generation++;

		for_replace = MIGRATION_RATE * population[0].deme_size;
		std::cout << "for replace " << for_replace << "\n";
		//for_replace_loosers = population[0].deme_size - for_replace_migration + 1;

		//paralelizar
		for (int i = 0; i < num_demes; i++) {
			deme_evolve(i);
		}

		for (int i = 0; i < num_demes; i++) {
			if (random_flip_coin(P_MIGRATION)) {
				ini = population[num_demes - 1].list_ind->end() - for_replace;
				end = population[num_demes - 1].list_ind->end();
				it = population[0].list_ind->begin();

				int cont = 0;

				for ( ; ini != end; ++ini) {
					*ini = *it;
					it++;
					cont++;
				}
				if (cont != for_replace) {
					std::cout << "no se copio todo " << cont << "\n";
				}
			}
		}

		if ((generation % FREQ_STATS) == 0) {
			std::cout << "\n==================================================\n";
			std::cout << "Generación #" << generation;
			std::cout << "\n==================================================\n";
			best_individual_in_training();
		}



	}
}

