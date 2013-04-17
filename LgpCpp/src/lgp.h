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

	//paralelizar
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


/*
 * def deme_evolve(population):
    '''Se reinician los indices, para hacer coincidir cada individuo.index con su posicion en internal_pop'''
    population = resetIndex(population)
    for gen in range(Parameters.gen_to_migrate):
        to_tournament = []
        '''Se seleccionan pool_size*2 diferentes posiciones en la poblacion para el torneo'''
        selected_indices = population.indices_selection(Parameters.pool_size * 2)

        to_tournament_indices = []
        to_tournament_indices.append(selected_indices[:Parameters.pool_size])
        to_tournament_indices.append(selected_indices[Parameters.pool_size:])
        to_tournament.append(population.selection_from_indices(to_tournament_indices[0]))
        to_tournament.append(population.selection_from_indices(to_tournament_indices[1]))

        #iter_result = population.pool.imap(Population.tournament_with_mutation, to_tournament, Parameters.chunk_size)
        winners = []
        ''' ********************************* MUTACION  *********************************'''
        for i in range(2):
            ''' Se retorna una lista de la siguiente forma: [modificado, no_modificado] '''
            result = Population.tournament_with_mutation(to_tournament[i])
            winners.append(result)
            #winners.append(iter_result.next())

        ''' ********************************* CROSSOVER *********************************'''
        if Util.random_flip_coin(Parameters.p_crossover):
            sister, brother = Population.crossover(winners[0][0], winners[1][0])
            Individual.check_destination_register(sister)
            Individual.check_destination_register(brother)

            if not sister.index == winners[0][0].index:
                winners[0][0] = brother
                winners[1][0] = sister

            if not Population.check_out_register(winners[0][0]) or not Population.check_out_register(winners[1][0]):
                print 'ERROR: [LgpMain.deme_evolve]: El crossover dejo individuos sin registros de salida.'

        for i in range(2):
            ''' Se elimina de la lista de participantes del torneo al ganador, para remplazar a los perdedores'''
            try:
                del to_tournament_indices[i][to_tournament[i].index(winners[i][1])]
            except:
                print "ERROR: [LgpMain.deme_evolve]: Error al eliminar el indice del ganador del torneo " + str(i)

            ''' best_replace = index_of_best([modificado, no_modificado])'''
            best_replace = 0 if Individual.compare(winners[i][0],winners[i][1]) == 1 else 1
            worst_replace = 1 if best_replace == 0 else 0

            ''' Se remplaza los perdedores por el mejor entre (ganador modificado, ganador NO modificado)
            y se actualizan los indices dentro de la población '''
            for l in to_tournament_indices[i]:
                indice = population.internal_pop[l].index
                population.internal_pop[l] = winners[i][best_replace].clone()
                population.internal_pop[l].index = indice
            '''
            Como ya se remplazo a los perdedores por el mejor, se remplaza al ganador por el peor. Hay mas copias del mejor.
            '''
            population.internal_pop[winners[i][0].index] = winners[i][worst_replace].clone().set_index(winners[i][0].index)

    '''Se ordena la población de mayor a menor, según el error promedio o la desviación típica
    según una cierta probabilidad'''
    if (Util.random_flip_coin(Parameters.p_migration_criteria)):
        (population.internal_pop).sort(cmp=Individual.compare_error_prom, reverse = True)
    else:
        (population.internal_pop).sort(cmp=Individual.compare_deviation_in_error, reverse = True)

    for i in population.internal_pop:
        if not i.evaluated:
            print "ERROR: METODO DE ORDENACION FALLO."

    return population
 */
