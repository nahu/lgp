/*
 * deme.h
 *
 *  Created on: Mar 16, 2013
 *      Author: Vanessa Cañete, Nahuel Hernández
 */

class Participant {
public:
	int pop_position;
	int status;
};


class Deme {
public:
	Deme();
	~Deme();
	//Deme(int size, int config_position);
	void create_new_deme(int size, int config_position);
	Individual * best_training();
	Individual * best_validation();
	void evaluate_individuals();

	//los métodos relacionados a population
	//void deme_evolve();
	Participant * indices_selection(unsigned int pool_size);
	Individual ** tournament_with_mutation(Participant * indices, int ini, int end);
	void override_loosers(Participant * indices, int ini, int end, Individual ** winners);

	std::vector<Individual> *list_ind;
	int deme_size;
};

Deme::Deme() {
	deme_size = 0;
	list_ind = 0;
}

void Deme::create_new_deme(int size, int config_position) {
	//std::cout<<"create_new_deme "<<size<<"  " <<config_position<<"\n";
	this->deme_size = size;
	//list_ind = new std::vector<Individual>(size, Individual(config_position));

	list_ind = new std::vector<Individual>(size);

	//todo paralelizar
	/*
	#pragma omp parallel for
	for (int it = 0; it < deme_size; ++it) {
		list_ind->at(it).create_new_individual(config_position);
	}
	*/

	int chunks = deme_size / (NUM_PROCESSORS);
	#pragma omp parallel for schedule(static, chunks)
	for (std::vector<Individual>::iterator it = list_ind->begin(); it < list_ind->end(); ++it) {
		(*it).create_new_individual(config_position);
	}


	//std::fill(deme_size, deme_size + size, Individual(config_position));
}

Deme::~Deme() {
	delete list_ind;
}


Individual* Deme::best_training() {
	std::vector<Individual>::iterator best = list_ind->begin();

	evaluate_individuals();

	for (std::vector<Individual>::iterator it = list_ind->begin() + 1; it != list_ind->end(); ++it) {
		if (Individual::compare_fitness(*best, *it)) {
			best = it;
		}
	}

	return &(*best);
}

Individual* Deme::best_validation() {
	std::vector<Individual>::iterator best = list_ind->begin();

	evaluate_individuals();

	for (std::vector<Individual>::iterator it = list_ind->begin(); it != list_ind->end(); ++it) {
		if (Individual::compare_validation_error(*best, *it)) {
			best = it;
		}
	}

	return &(*best);
}


void Deme::evaluate_individuals() {
	//todo paralelizar

	//int chunks = deme_size / (NUM_PROCESSORS * 16);
	//#pragma omp parallel for schedule(guided, chunks)

	//int chunks = deme_size / (NUM_PROCESSORS * 16);
	//#pragma omp parallel for schedule(dynamic, chunks)
	for (std::vector<Individual>::iterator it = list_ind->begin(); it < list_ind->end(); ++it) {
		(*it).eval_fitness();
	}
}

Participant * Deme::indices_selection(unsigned int pool_size) {
	Participant * index_pool = new Participant[pool_size];
	std::set<int> pool_set;
	int i = 0, index;

	while (pool_set.size() < pool_size) {
		index = randint(0, deme_size - 1);
		if (index == deme_size) {
			std::cout<<"Problemas deme_size = index "<<"\n";
		}
		pool_set.insert(index);
	}

	for (std::set<int>::iterator it = pool_set.begin(); it != pool_set.end(); ++it) {
		index_pool[i].pop_position = *it;
		index_pool[i].status = NONE;
		i++;
	}

	//std::copy(pool_set.begin(), pool_set.end(), std::back_inserter(index_pool));

	return index_pool;
}



Individual** Deme::tournament_with_mutation(Participant * indices, int ini, int end) {
	Individual** r = new Individual*[2];
	Individual * winner;
	int choosen = ini;

	/*
	std::cout<<"to replace\n";
	std::cout<<"ini " << ini << "   ";
	std::cout<<"end " << end << "\n";
	*/

	int pos = indices[choosen].pop_position;
	int to_replace[POOL_REPRODUCTION];
	to_replace[0] = ini;

	int tamanho = list_ind->size();
	if ((tamanho != deme_size) || (deme_size < pos)) {
		std::cout<<"Problemas deme_size "<< deme_size << ", tamanho " << tamanho << ", pos " << pos <<"\n";
	}


	//std::cout<<"Problemas deme_size "<< deme_size << ", tamanho " << tamanho << ", pos " << pos <<"\n";
	list_ind->at(pos).eval_fitness();

	int replace_count = 1;

	for (int i = ini + 1; i < end; i++) {

		list_ind->at(indices[i].pop_position).eval_fitness();
		//std::cout<<"(*it).pop_position "<<(*it).pop_position<<"\n";
		//std::cout<<"(*chosen).pop_position "<<(*choosen).pop_position<<"\n";
		if (Individual::compare_fitness(list_ind->at(indices[choosen].pop_position), list_ind->at(indices[i].pop_position))) {
			choosen = i;
		}

		if (replace_count < POOL_REPRODUCTION) {
			to_replace[replace_count] = i;
			replace_count++;
		} else {
			for (int j = 0; j < POOL_REPRODUCTION; j++) {
				if (Individual::compare_fitness(list_ind->at(indices[i].pop_position), list_ind->at(indices[to_replace[j]].pop_position))) {
					to_replace[j] = i;
					break;
				}
			}
		}

	}



	//perdedores a sobreescribir por el mejor ganador (modificado/sin modificar)
	for (int j = 0; j < POOL_REPRODUCTION; j++) {
		indices[to_replace[j]].status = REPLACE;
	}

	//establecer los estados de los participantes
	indices[choosen].status = WINNER; //ganador

	winner = new Individual(list_ind->at(indices[choosen].pop_position));


	if (random_flip_coin(P_MACRO_MUTATION)) {
		winner->macro_mutation();
	}

	if (random_flip_coin(P_MICRO_MUTATION)) {
		winner->micro_mutation();
	}


	r[0] = winner;
	r[1] = &list_ind->at(indices[choosen].pop_position);

	return r;
}



//Se remplaza los perdedores por el mejor entre (ganador modificado, ganador NO modificado) y se actualizan los indices dentro de la población
void Deme::override_loosers(Participant * indices, int ini, int end, Individual ** winners) {
	int best_replace, worst_replace;
	int win_pos;
	//std::cout<<"modificado\n";
	//winners[0]->print_individual();
	winners[0]->eval_fitness();
	//std::cout<<"original\n";
	winners[1]->eval_fitness();

	best_replace = 1;
	if (Individual::compare_fitness(*(winners[1]), *(winners[0]))) {
		best_replace = 0;
	}

	//std::cout<<"override_loosers 172\n";

	worst_replace = 0;
	if (best_replace == 0) {
		worst_replace = 1;
	}

	for ( ;ini < end; ++ini) {
		if (indices[ini].status == WINNER) {
			win_pos = indices[ini].pop_position;
		} else if (indices[ini].status == REPLACE){
			list_ind->at(indices[ini].pop_position) = *(winners[best_replace]);
		}
	}

	list_ind->at(win_pos) = *(winners[worst_replace]);
}
