/*
 * deme.h
 *
 *  Created on: Mar 16, 2013
 *      Author: Vanessa Cañete, Nahuel Hernández
 */

class Participant {
public:
	int pop_position;
	bool win;
};

typedef std::vector<Participant>::iterator participant_iter;

class Deme { //esto es population
public:
	Deme();
	~Deme();
	//Deme(int size, int config_position);
	void create_new_deme(int size, int config_position);
	Individual * best_training();
	Individual * best_validation();
	void evaluate_individuals();

	//los métodos relacionados a population
	void deme_evolve();
	std::vector<Participant> indices_selection(unsigned int pool_size);
	Individual ** tournament_with_mutation(std::vector<Participant> &indices, participant_iter ini, participant_iter end);
	void override_loosers(std::vector<Participant> &indices, participant_iter ini, participant_iter end, Individual ** winners);

	std::vector<Individual> *list_ind;
	int deme_size;
};

Deme::Deme() {
	deme_size = 0;
	list_ind = 0;
}

void Deme::create_new_deme(int size, int config_position) {
	this->deme_size = size;

	//list_ind = new std::vector<Individual>(size, Individual(config_position));

	list_ind = new std::vector<Individual>(size);

	//paralelizar
	for (std::vector<Individual>::iterator it = list_ind->begin(); it != list_ind->end(); ++it) {
		(*it).create_new_individual(config_position);
	}

	//std::fill(deme_size, deme_size + size, Individual(config_position));
}

Deme::~Deme() {
	delete list_ind;
}


Individual* Deme::best_training() {
	std::vector<Individual>::iterator best = list_ind[0].begin();

	evaluate_individuals();

	for (std::vector<Individual>::iterator it = list_ind->begin() + 1; it != list_ind->end(); ++it) {
		if (Individual::compare_fitness(*best, *it)) {
			best = it;
		}
	}

	return &(*best);
}

Individual* Deme::best_validation() {
	std::vector<Individual>::iterator best = list_ind[0].begin();

	evaluate_individuals();

	for (std::vector<Individual>::iterator it = list_ind->begin(); it != list_ind->end(); ++it) {
		if (Individual::compare_validation_error(*best, *it)) {
			best = it;
		}
	}

	return &(*best);
}


void Deme::evaluate_individuals() {
	//paralelizar
	for (std::vector<Individual>::iterator it = list_ind->begin(); it != list_ind->end(); ++it) {
		(*it).eval_fitness();
	}
}


std::vector<Participant> Deme::indices_selection(unsigned int pool_size) {
	std::vector<Participant> index_pool(pool_size);
	std::set<int> pool_set;
	int i = 0, index;

	while (pool_set.size() < pool_size) {
		index = randint(0, deme_size - 1);
		pool_set.insert(index);
	}

	for (std::set<int>::iterator it = pool_set.begin(); it != pool_set.end(); ++it) {
		index_pool[i].pop_position = *it;
		index_pool[i].win = false;
		i++;
	}

	//std::copy(pool_set.begin(), pool_set.end(), std::back_inserter(index_pool));

	return index_pool;
}



Individual** Deme::tournament_with_mutation(std::vector<Participant> &indices, participant_iter ini, participant_iter end) {
	Individual ** competitors = new Individual*[POOL_SIZE];
	Individual** r = new Individual*[2];
	Individual * winner;
	int win_pos = 0;
	int i = 0;
	participant_iter it, choosen = ini;

	for (it = ini + 1; it != end; ++it) {
		if (Individual::compare_fitness(*(list_ind[(*it).pop_position].data()), *(list_ind[(*(choosen)).pop_position].data()))) {
			choosen = it;
		}
	}

	(*choosen).win = true;

	winner = new Individual(*(list_ind[(*choosen).pop_position].data()));


	//faltan las mutaciones
	if (random_flip_coin(P_MACRO_MUTATION)) {
		winner->macro_mutation();
	}

	if (random_flip_coin(P_MICRO_MUTATION)) {
		winner->micro_mutation();
	}


	r[0] = winner;
	r[1] = list_ind[(*choosen).pop_position].data();

	return r;
}



//Se remplaza los perdedores por el mejor entre (ganador modificado, ganador NO modificado) y se actualizan los indices dentro de la población
void Deme::override_loosers(std::vector<Participant> &indices, participant_iter ini, participant_iter end, Individual ** winners) {
	int best_replace, worst_replace;
	int win_pos;

	best_replace = 1;
	if (Individual::compare_fitness(*(winners[0]), *(winners[1]))) {
		best_replace = 0;
	}

	worst_replace = 1 ? best_replace == 0 : 0;

	for ( ;ini != end; ++ini) {
		if ((*ini).win) {
			win_pos = (*ini).pop_position;
		} else {
			*(list_ind[(*ini).pop_position].data()) = *(winners[best_replace]);
		}
	}

	*(list_ind[win_pos].data()) = *(winners[worst_replace]);

}
