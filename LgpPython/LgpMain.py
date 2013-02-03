# -*- coding: utf-8 -*-

"""
Módulo que define el algoritmo LGP

@authors:
- U{Nahuel Hernández<mailto:jnahuelhernandez@gmail.com>}
- U{Vanessa Cañete<mailto:vanessa.can.89@gmail.com>}

@since: 1.0
"""

from multiprocessing import Pool
from multiprocessing import Manager
from datetime import datetime

import Population
import Util
import Parameters
import copy
import Individual
import time


def sum_errors(lista):
    error_total = 0
    
    for x in lista:
        error_total += x
    lista.append(error_total)
    lista.append(error_total/Parameters.validation_lines)
    
    return lista


def step_by_pool_size(population):
    migrators = []
    loosers = set([])
    
    for generation in range(Parameters.pool_size - 1):
        to_tournament = []
        selected_indices = population.indices_selection(Parameters.pool_size * 2)
        
        to_tournament_indices = []
        to_tournament_indices.append(selected_indices[:Parameters.pool_size])
        to_tournament_indices.append(selected_indices[Parameters.pool_size:])
        to_tournament.append(population.selection_from_indices(to_tournament_indices[0]))
        to_tournament.append(population.selection_from_indices(to_tournament_indices[1]))
        
        iter = population.pool.imap(Population.tournament_with_mutation, to_tournament, Parameters.chunk_size)
        
        
        """
        Se retorna una lista de la siguiente forma:
            [0] El ganador modificado
            [1] El ganador sin modificar
        """
        winners = []
        for i in range(2):
            winners.append(iter.next())
        
        
        if Util.random_flip_coin(Parameters.p_crossover):
            winners[0][0], winners[0][1] = Population.crossover(winners[0][0], winners[0][1])
                
            if not self.check_out_register(winners[0][0]) or not self.check_out_register(winners[0][1]):
                print "El crossover mató"
                
        for i in range(2):
            '''
            Se elimina de la lista de participantes del torneo al ganador, 
            para remplazar a los perdedores
            '''
            to_tournament_indices[i].remove(winners[i].index)
            
            '''
            Se guardan los índices de los perdedores para hacer un posterior ordenamiento,
            los peores serán reemplazados por los individuos que migren del deme
            adyacente
            '''
            [loosers.add(j) for j in to_tournament_indices[i]]
            
            '''
            Se setean las copias temporales en las posiciones de los perdedores del torneo
            y se actualiza el índice dentro de la población
            '''
            for l in to_tournament_indices[i]:
                population.internal_pop[l] = winners[1][i].clone()
                population.internal_pop[l].index = l
        
            '''
            Se remplaza a los ganadores del torneo por los nuevos individos operados genéticamente
            '''
            self.population.internal_pop[winners[0][i].index] = winners[0][i]
            
        '''
        Se selecciona el mejor de ambos ganadores para sin modificación para la migración
        '''
        if (Individual.compare(winners[1][0], winners[1][1]) == 1):
            migrators.append(winners[1][0])
        else:
            migrators.append(winners[1][1])
            
    return (migrators, population, list(loosers))
        
       
            
class LGP():
    def __init__(self, config_position=0, pool=None, demes=1):
        if not pool:
            self.pool = Pool(processes=Parameters.num_processors)
        else:
            self.pool = pool
            
        self.generation = 0
        self.config_position = config_position
        self.num_demes = demes
        self.population = []
        
    def terminate(self):
        pass
        """
        self.pool.close()
        self.pool.join()
        """
        
    def set_population_size(self, population_size):
        div = population_size // self.num_demes
        pop_size_per_deme = div if div > 0 else 1
        
        for i in range(self.num_demes):
            self.population.append(Population.Population(pop_size_per_deme, self.config_position))
        
    
    def initialize_pop(self):
        '''
        Inicializa en paralelo los demes
        '''
        
        for pop in range(self.num_demes):
            iter = self.pool.imap(Individual.ini_individual, self.population[pop].internal_pop, Parameters.chunk_size)
        
            for individual in range(self.population[pop].pop_size):
                self.population[pop].internal_pop[individual] = iter.next()
         
         
         
    def set_num_generations(self, generations):
        self.num_generations = generations
        
        
    """
    def step(self):
        to_tournament = []
        selected_indices = self.population.indices_selection(Parameters.pool_size * 2)
        
        to_tournament_indices = []
        to_tournament_indices.append(selected_indices[:Parameters.pool_size])
        to_tournament_indices.append(selected_indices[Parameters.pool_size:])
        to_tournament.append(self.population.selection_from_indices(to_tournament_indices[0]))
        to_tournament.append(self.population.selection_from_indices(to_tournament_indices[1]))
        
        iter = self.pool.imap(Population.tournament, to_tournament, Parameters.chunk_size)

        winners = []
        for i in range(2):
            winners.append(iter.next())
        
        
        '''Se hacen copias temporales para remplazar luego a los perdedores del torneo'''
        winners_tmp = []
        for w in winners:
            winners_tmp.append(w.clone())
        
        if Util.random_flip_coin(Parameters.p_crossover):
            winners[0], winners[1] = Population.crossover(winners[0], winners[1])
            
        if not self.check_out_register(winners[0]) or not self.check_out_register(winners[1]):
            print "El crossover mató"
            
        for i in range(2):
            try:
                if Util.random_flip_coin(Parameters.p_macro_mutation):
                    winners[i] = Population.macro_mutation(winners[i])
            
                if not self.check_out_register(winners[i]):
                    print "La macro mató"
            except:
                print "La macro matOOOó"
                 
            if Util.random_flip_coin(Parameters.p_micro_mutation):
                winners[i] = Population.micro_mutation(winners[i])
            try:   
                if not self.check_out_register(winners[i]):
                    print "La miiicro mató"
            except:
                print "La miiicro mató"
                
            '''Se elimina de la lista de participantes del torneo al ganador, para remplazar a los perdedores'''
            to_tournament_indices[i].remove(winners[i].index)
            
            '''Se setean las copias temporales en las posiciones de los perdedores del torneo
            y se actualiza el indice dentro de la poblacion'''
            for l in to_tournament_indices[i]:
                self.population.internal_pop[l] = winners_tmp[i].clone()
                self.population.internal_pop[l].index = l
            
            '''Se remplaza a los ganadores del torneo por los nuevos individos operados geneticamente'''    
            self.population.internal_pop[winners[i].index] = winners[i]
    """
        
    def termination_criteria(self):
        return self.generation < self.num_generations
    
    
    def check_out_register(self, individual):
        return individual.genomeList[individual.height - 1][1] == 0
    
    

    def evolve(self, freq_stats=-1):
        #Se inicializan todos los demes
        self.initialize_pop()
        self.generation = 0
        
        try:
            while not self.termination_criteria():
                self.generation += Parameters.pool_size
                iter = self.pool.imap(step_by_pool_size, self.population, Parameters.chunk_size)
                
                '''
                iter:
                    [0] Pool_size - 1 Migrators
                    [1] Population changed
                    [2] Indices for replace
                '''
                migrators = []
                indices = []
                tmp_populations = []
                last_migrators, first_population, firts_indices = iter.next()
                migrators.append(last_migrators)
                tmp_populations.append(first_population)
                indices.append(firts_indices)
                
                for p in range(1, self.num_demes):
                    mig, pop, ind = iter.next()
                    migrators.append(mig)
                    tmp_populations.append(pop)
                    indices.append(ind)
                    
                    to_del = self.population[p]
                    self.population[p] = tmp_populations[p]
                    del to_del
                    
                    '''
                    Se seleccionan los pool_size -1 individuos que tengan menor fitness
                    entre los perdedores de los pool_size torneos, para ser
                    reemplazados por los migrators de la población adyacente
                    '''
                    loosers = self.population[p].selection_from_indices(indices[p])
                    loosers.sort(cmp=Individual.compare)
                    
                    '''
                    Se sobreescriben los perdedores y actualizan los índices
                    '''
                    if len(migrators[p-1]) != Parameters.pool_size:
                        print 'numero de migrators ERROR'
                        
                    for l in range(Parameters.pool_size):
                        self.population[p][loosers[l].index] = migrators[p-1][l]
                        self.population[p][loosers[l].index].index = loosers[l].index
                
                '''
                Configuración de adyacencia en anillo, los últimos migrators van
                a la primera población
                '''
                to_del = self.population[0]
                self.population[0] = tmp_populations[0]
                del to_del
                
                loosers = self.population[0].selection_from_indices(indices[0])
                loosers.sort(cmp=Individual.compare)
                for l in range(Parameters.pool_size):
                    self.population[0][loosers[l].index] = migrators[self.num_demes - 1][l]
                    self.population[0][loosers[l].index].index = loosers[l].index
                    
            
                
                stats = self.generation % freq_stats
                
                if stats > freq_stats - 1:
                    print "Generación " + str(self.generation) + " ...."
                
        except KeyboardInterrupt:
            print "Terminando Workers..."
            self.terminate()
            exit()
        
        
    def best_individual_in_training(self):
        deme_best = []
        
        iter = self.pool.imap(Population.best_training_in_pop, self.population, Parameters.chunk_size)
        
        print "Los 5 mejores en Entrenamiento"
        for deme in range(self.num_demes):
            best = iter.next()
            deme_best.append(best)
            print "Deme " + str(deme) + ", individuo " + str(best.index) + " " + str(1.0/best.fitness) 
            
        deme_best.sort(cmp=Individual.compare, reverse=True)
        
        return deme_best[0]
    
    def best_individual_in_validation(self):
        deme_best = []
        
        iter = self.pool.imap(Population.best_validation_in_pop, self.population, Parameters.chunk_size)
        
        print "Los 5 mejores en Validación"
        for deme in range(self.num_demes):
            best = iter.next()
            deme_best.append(best)
            print "Deme " + str(deme) + ", individuo " + str(best.index) + " " + str(best.validation_error)
            
        deme_best.sort(cmp=Individual.compare_error)
        
        return deme_best[0]
        


if __name__ == "__main__":
#    print Parameters.data_samples
#    print Parameters.r_const

    t_inicio = time.clock()
    pool = Pool(processes=Parameters.num_processors)
    
    positions = []
    best_individuals = []
    '''
    for i in range(Parameters.n):
        if Parameters.config[i] == '0':
            #positions.append(i)
    '''
    i = 1
    t1 = time.clock()
    print "Transformador " + str(i)
    positions.append(i)
    ga = LGP(config_position=i, pool=pool, demes=Parameters.demes)
    ga.set_num_generations(Parameters.num_generations)
    ga.set_population_size(Parameters.population_size)
    ga.evolve(freq_stats=Parameters.freq_stats)
    
    best = ga.best_individual_in_training()
    best2 = ga.best_individual_in_validation()
    ga.terminate()
    
    best_individuals.append(best)
    t2 = time.clock()
    print '%s Duracion %0.5f s' % ("Transf. " + str(i), (t2-t1))
    print "\n"
    
    iter = pool.imap(Individual.eval_individual, best_individuals, Parameters.chunk_size)
    
    diff =  str(datetime.now())
    diff = "-" + diff.replace(':', "-")
    
    final_table = []
    for i in positions:
        errors_i = iter.next()
        final_table.append(errors_i)
        
    for j in range(0, len(final_table)):
        errors_and_sum = sum_errors(final_table[j])
        errors_and_sum.append(positions[j])
        final_table[j] = errors_and_sum
        
        
    f_errors = "errores" + "-G" + str(Parameters.num_generations) + "_P" + str(Parameters.population_size) + diff + ".csv"
    f = open(f_errors, "w")
        
    for t in range(Parameters.validation_lines + 3):
        if t == Parameters.validation_lines:
            row = "error total"
        elif t ==  Parameters.validation_lines + 1:
            row = "error promedio"
        elif t ==  Parameters.validation_lines + 2:
            row = "posicion"
        else:
            row = str(Parameters.training_lines + t) + ";"
        for i in range(len(final_table)):
            row += (str(final_table[i][t]) + ";")
        row += "\n"
        f.write(row.replace('.', ','))
    
    f_programs = "programas" + "-G" + str(Parameters.num_generations) + "_P" + str(Parameters.population_size)+ diff +".txt"
    g = open(f_programs, "w")
    for b in best_individuals:
        g.write(repr(b))
        g.write("\n")
        g.write(b.get_program_in_python())
        g.write("\n\n\n")
        
        
    g.close()
    f.close()
    pool.close()
    pool.join()
    t_final = time.clock()
    print '%s Duracion %0.5f s' % ("LGPMAIN", (t_final - t_inicio))
    print "\n"
