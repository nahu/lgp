/*
 * lgp.h
 *
 *  Created on: Mar 16, 2013
 *      Author: Vanessa Cañete, Nahuel Hernández
 */


//toda la parte de LGPMAIN.py que están dentro del la clase lgpmain y tiene el algoritmo evolutivo en sí

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
