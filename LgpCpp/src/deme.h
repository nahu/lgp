/*
 * deme.h
 *
 *  Created on: Mar 16, 2013
 *      Author: Vanessa Cañete, Nahuel Hernández
 */


class Deme { //esto es population
public:
	Deme(int size, int config_position);
	Deme();
	~Deme();
	Individual *list_ind;
	int deme_size;

	//los métodos relacionados a population

	void deme_evolve();


};

Deme::Deme() {
	deme_size = 0;
	list_ind = 0;
}

Deme::Deme(int size, int config_position) {
	this->deme_size = size;

	list_ind = new Individual[size];
	for (int i =0; i < size; i++) {
		list_ind[i] = Individual(config_position, i);
	}
	//std::fill(deme_size, deme_size + size, Individual(config_position));
}

Deme::~Deme() {
	delete [] list_ind;
}

/*
 *
 * def create(self, **args):
        for i in xrange(self.pop_size):
            self.internal_pop.append(Individual.Individual(4, i, self.config_position))


    def indices_selection(self, pool_size):
        index_pool = []

        while len(index_pool) < pool_size:
            index = random.randint(0, self.pop_size - 1)
            if not (index in index_pool):
                index_pool.append(index)

        return index_pool


    def selection_from_indices(self, indices):
        pool = []

        for i in indices:
            pool.append(self.internal_pop[i])

        return pool


    def best_training(self):
        best = self.internal_pop[0]

        for i in range(1, self.pop_size):
            if (Individual.compare(self.internal_pop[i], best) == 1):
                best = self.internal_pop[i]

        return best

    def best_validation(self):
        '''
        Se establece el atributo validation error de los individuos de la población
        '''
        for i in self.internal_pop:
            i.eval_validation()

        best = self.internal_pop[0]

        '''
        Se busca el de menor error
        '''
        for i in range(1, self.pop_size):
            if (Individual.compare_validation_error(self.internal_pop[i], best) == -1):
                best = self.internal_pop[i]

        return best
 * def tournament_with_mutation(competitors):
    choosen = competitors[0]

    for i in range(1, len(competitors)):
        if Individual.compare(choosen, competitors[i]) == -1:
            choosen = competitors[i]

    '''
    Se hacen copias temporales para reemplazar luego a los perdedores del torneo
    '''
    winner = choosen.clone()

    '''
    Se realizan la macro y micro mutación según probabilidades
    '''
    try:
        if Util.random_flip_coin(Parameters.p_macro_mutation):
            winner = macro_mutation(winner)
            Individual.check_destination_register(winner)
        if not check_out_register(winner):
            print "La macro mató"
    except:
        print "La macro matOOOó"

    try:
        if Util.random_flip_coin(Parameters.p_micro_mutation):
            winner = micro_mutation(winner)
            Individual.check_destination_register(winner)

        if not check_out_register(winner):
            print "La miiicro mató"
    except:
        print "La miiicro mató"

    return list([winner, choosen])



 */
