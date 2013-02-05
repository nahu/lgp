# -*- coding: utf-8 -*-

"""
Módulo que define el algoritmo LGP

@authors:
- U{Nahuel Hernández<mailto:jnahuelhernandez@gmail.com>}
- U{Vanessa Cañete<mailto:vanessa.can.89@gmail.com>}

@since: 1.0
"""
from itertools import imap
from multiprocessing import Pool
from multiprocessing import Manager
from datetime import datetime

import os
import copy
import time
import types
import Population

import Util

import Parameters
import Individual



def sum_errors(list_of_errors):
    error_total = 0
    
    for x in list_of_errors:
        error_total += x
    list_of_errors.append(error_total)
    list_of_errors.append(error_total/Parameters.validation_lines)
    
    return list_of_errors


def step_by_pool_size(population):
    migrators = []
    loosers = set([])
    
    for generation in range(Parameters.pool_size):
        to_tournament = []
        selected_indices = population.indices_selection(Parameters.pool_size * 2)
        
        to_tournament_indices = []
        to_tournament_indices.append(selected_indices[:Parameters.pool_size])
        to_tournament_indices.append(selected_indices[Parameters.pool_size:])
        to_tournament.append(population.selection_from_indices(to_tournament_indices[0]))
        to_tournament.append(population.selection_from_indices(to_tournament_indices[1]))
        
        #iter = population.pool.imap(Population.tournament_with_mutation, to_tournament, Parameters.chunk_size)
        
        
        """
        Se retorna una lista de la siguiente forma:
            [0] El ganador modificado
            [1] El ganador sin modificar
        """
        winners = []
        for i in range(2):
            result = Population.tournament_with_mutation(to_tournament[i])
            winners.append(result)
            #winners.append(iter.next())
        
        
        if Util.random_flip_coin(Parameters.p_crossover):
            sister, brother = Population.crossover(winners[0][0], winners[1][0])
            
            if not sister.index == winners[0][0].index:
                winners[0][0] = brother
                winners[1][0] = sister
                
            if not Population.check_out_register(winners[0][0]) or not Population.check_out_register(winners[1][0]):
                print "El crossover mató"
                
        for i in range(2):
            '''
            Se elimina de la lista de participantes del torneo al ganador, 
            para remplazar a los perdedores
            '''
            try:
                to_tournament_indices[i].remove(winners[i][1].index)
            except:
                
                print "remove error"
            #return to_tournament_indices[i], to_tournament_indices[1 if i == 0 else 0], winners[i][1].index
            
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
                population.internal_pop[l] = winners[i][1].clone()
                population.internal_pop[l].index = l
        
            '''
            Se remplaza a los ganadores del torneo por los nuevos individos operados genéticamente
            '''
            population.internal_pop[winners[i][0].index] = winners[i][0].clone()
            
        '''
        Se selecciona el mejor de ambos ganadores para reproducir sin modificación para la migración
        '''
        if (Individual.compare(winners[0][1], winners[1][1]) == 1):
            migrators.append(winners[0][1])
        else:
            migrators.append(winners[1][1])
            
    return list([migrators, population, list(loosers)])
        
       
            
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
        
        
    def termination_criteria(self):
        return self.generation > self.num_generations
    
    
    

    def evolve(self, freq_stats=-1):
        #Se inicializan todos los demes
        self.initialize_pop()
        self.generation = 0
        
        try:
            while not self.termination_criteria():
                
                self.generation += Parameters.pool_size
                iter = self.pool.imap(step_by_pool_size, self.population, 1)
                
                '''
                iter:
                    [0] Pool_size - 1 Migrators
                    [1] Population changed
                    [2] Indices for replace
                '''
               
                migrators = []
                indices = []
                tmp_populations = []
                result = iter.next()#step_by_pool_size(self.population[0])#
                last_migrators = result[0]
                first_population = result[1]
                firts_indices = result[2] 
                '''
                print last_migrators
                print first_population
                print firts_indices
                '''
                migrators.append(last_migrators)
                tmp_populations.append(first_population)
                indices.append(firts_indices)
                
                for p in range(1, self.num_demes):
                    result = iter.next() #step_by_pool_size(self.population[p])
                    mig = result[0]
                    pop = result[1]
                    ind = result[2] 
                    migrators.append(mig)
                    tmp_populations.append(pop)
                    indices.append(ind)
                    
                    to_del = self.population[p]
                    self.population[p] = copy.deepcopy(tmp_populations[p])
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
                        indice = loosers[l].index
                        self.population[p].internal_pop[indice] = migrators[p-1][l].clone()
                        self.population[p].internal_pop[indice].index = indice
                    #print "estado"
                
                '''
                Configuración de adyacencia en anillo, los últimos migrators van
                a la primera población
                '''
                to_del = self.population[0]
                self.population[0] = copy.deepcopy(tmp_populations[0])
                del to_del
                
                loosers = self.population[0].selection_from_indices(indices[0])
                loosers.sort(cmp=Individual.compare)
                for l in range(Parameters.pool_size):
                    indice = loosers[l].index
                    self.population[0].internal_pop[indice] = migrators[self.num_demes - 1][l].clone()
                    self.population[0].internal_pop[indice].index = indice
            
                
                stats = self.generation % freq_stats
                
                if (freq_stats - stats) <= Parameters.pool_size:
                    print "Generación " + str(self.generation) + " ...."
                
        except KeyboardInterrupt:
            print "Terminando Workers..."
            self.terminate()
            exit()
        
        
    def best_individual_in_training(self):
        deme_best = []
        
        iter = self.pool.imap(Population.best_training_in_pop, self.population, 2)
        
        print "Los " + str(self.num_demes) + " mejores en Entrenamiento..."
        for deme in range(self.num_demes):
            best = iter.next()
            deme_best.append(best)
            print "Deme " + str(deme) + ", individuo " + str(best.index) + " " + str(1.0/best.fitness) 
            
        deme_best.sort(cmp=Individual.compare, reverse=True)
        
        return deme_best[0]
    
    def best_individual_in_validation(self):
        deme_best = []
        
        iter = self.pool.imap(Population.best_validation_in_pop, self.population, 2)
        
        print "Los  " + str(self.num_demes) + " mejores en Validación..."
        for deme in range(self.num_demes):
            best = iter.next()
            deme_best.append(best)
            print "Deme " + str(deme) + ", individuo " + str(best.index) + " " + str(best.validation_error)
            
        deme_best.sort(cmp=Individual.compare_error)
        
        return deme_best[0]
        
        
def errors_to_file(f_errors, final_table):
    f = open(f_errors, "w")

    for t in range(Parameters.validation_lines + 3):
        if t == Parameters.validation_lines:
            row = "Error total;"
        elif t ==  Parameters.validation_lines + 1:
            row = "Error promedio;"
        elif t ==  Parameters.validation_lines + 2:
            row = "Posicion;"
        else:
            row = str(Parameters.training_lines + t) + ";"
        for i in range(len(final_table)):
            row += (str(final_table[i][t]) + ";")
        row += "\n"
        f.write(row.replace('.', ','))
        
    f.close()



def programs_to_file(f_programs, best_individuals):
    g = open(f_programs, "w")
    
    for b in best_individuals:
        g.write(repr(b))
        g.write("\n")
        g.write("#Effective Program:\n")
        g.write(b.get_program_in_python())
        g.write("\n\n\n")
        
    g.close()


def parameters_to_file(f_param):
    f = open(f_param, "w")
    
    ps =  [(a + " = " + str(Parameters.__dict__.get(a)) + "\n") for a in dir(Parameters) 
           if (isinstance(Parameters.__dict__.get(a), types.FloatType) or 
               isinstance(Parameters.__dict__.get(a), types.IntType))]
    for l in ps:
        f.write(l)
        
    f.close()
    
    
if __name__ == "__main__":
#    print Parameters.data_samples
#    print Parameters.r_const
    
    t_inicio = time.clock()
    pool = Pool(processes=Parameters.num_processors)
    
    positions = [15,23]
    
    
    diff = str(datetime.now())
    dir = "resultados/"
    dir += diff[:19].replace(':', '.')
    '''   
    for i in range(Parameters.n):
        if Parameters.config[i] == '0':
            positions.append(i)
    '''
    for i in positions:
        #i = 3
        best_individuals = []
        t1 = time.clock()
        print "Transformador " + str(i)
        #positions.append(i)
        ga = LGP(config_position=i, pool=pool, demes=Parameters.demes)
        ga.set_num_generations(Parameters.num_generations)
        ga.set_population_size(Parameters.population_size)
        ga.evolve(freq_stats=Parameters.freq_stats)
        
        best_training = ga.best_individual_in_training()
        best_validation = ga.best_individual_in_validation()
        ga.terminate()
        
        best_individuals.append(best_training)
        best_individuals.append(best_validation)
        
        t2 = time.clock()
        print "\n"
        print '%s Duracion %0.5f s' % ("Transf. " + str(i), (t2-t1))
        print "\n"
        
        iter = pool.imap(Individual.eval_individual, best_individuals, 1)
        
        final_table = []
        '''
        final_table[0] errores de validación con el mejor individuo de entrenamiento
        final_table[1] errorer de validación con el mejor individuo con datos de validación
        '''
        for j in range(2):
            errors_j = iter.next()
            errors_and_sum = sum_errors(errors_j)
            errors_and_sum.append(i)
            final_table.append(errors_and_sum)
            
        if not os.path.exists(dir):
            os.makedirs(dir)
            
        f_errors = dir + "/errores-TRAF" + str(i) + "-G" + str(Parameters.num_generations) + "_P" + str(Parameters.population_size) + ".csv"
        errors_to_file(f_errors, final_table)
        f_programs = dir + "/programas-TRAF" + str(i) + "-G" + str(Parameters.num_generations) + "_P" + str(Parameters.population_size) + ".txt"
        programs_to_file(f_programs, best_individuals)
        '''
        f_parameters = dir + "/parameters-TRAF" + str(i)
        parameters_to_file(f_parameters)
        '''
    
    pool.close()
    pool.join()
    
    t_final = time.clock()
    print '%s Duracion %0.5f s' % ("LGPMAIN", (t_final - t_inicio))
    print "\n"
    raw_input()
