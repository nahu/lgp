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
	Individual * best_individual_in_training();
	Individual ** best_individuals_of_demes(int tipo);
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
	//std::cout<<"Const. LGP\n";

	generation = 0;
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


Individual* Lgp::best_individual_in_training() {
	Individual ** deme_best = new Individual *[num_demes];
	Individual * best = 0;

	//todo: paralelizar
	for (int i = 0; i < num_demes; i++) {
		deme_best[i] = population[i].best_training();
	}

	best = deme_best[0];

	for (int i = 1; i < num_demes; i++) {
		if (Individual::compare_fitness(*best, *deme_best[i])) {
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
	return (generation > num_generation);
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
	int for_replace;
	std::vector<Individual>::iterator ini, end, it;

	while (!termination_criteria()) {
		generation++;
		//std::cout << "Generación #" << generation << "\n";
		for_replace = MIGRATION_RATE * (float) population[0].deme_size;
		//for_replace = 3;
		//std::cout << "for replace " << for_replace << "\n";

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
				if ((*ini)
				(*ini) = (*it);


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
					(*ini) = (*it);

					(*ini) = (*it);
					it++;
					cont++;
				}
				//std::cout << "migrados: " << cont << "\n";
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
	best_individual_in_training();
}

