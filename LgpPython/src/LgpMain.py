# -*- coding: utf-8 -*-

"""
Módulo que define el algoritmo LGP

@authors:
- U{Nahuel Hernández<mailto:jnahuelhernandez@gmail.com>}
- U{Vanessa Cañete<mailto:vanessa.can.89@gmail.com>}

@since: 1.0
"""

from multiprocessing import Pool
from datetime import datetime

import os
import sys
import copy
import time
import types

import Population
import Util
import Parameters
import Individual
from MdpProcess import * 

def deme_evolve(population):
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

def resetIndex(population):
    for j in range(population.pop_size):
        population.internal_pop[j].index = j
    return population
       
            
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
#        for pop in range(self.num_demes):
#            for i in range(len(self.population[pop].internal_pop)):
#                self.population[pop].internal_pop[i] = Individual.ini_individual(self.population[pop].internal_pop[i])
        for pop in range(self.num_demes):
            '''
            Parameters.chunk_size sería la cantidad de objetos que se le pasa a un worker para que le aplique la función
            Individual.ini_individual
            '''
            iter_result = self.pool.imap(Individual.ini_individual, self.population[pop].internal_pop, Parameters.chunk_size)
        
            for individual in range(self.population[pop].pop_size):
                self.population[pop].internal_pop[individual] = iter_result.next()
         
         
         
    def set_num_generations(self, generations):
        self.num_generations = generations
        
        
    def termination_criteria(self):
        return self.generation > self.num_generations
    
    
    

    def evolve(self, freq_stats=-1):
        self.initialize_pop()
        self.generation = -1
        
        try:
            while not self.termination_criteria():
                for_replace_loosers = int(self.population[0].pop_size - Parameters.migration_rate * self.population[0].pop_size) + 1
                for_replace_migration = int(Parameters.migration_rate * self.population[0].pop_size)
                self.generation += 1
                
                iter_result = self.pool.imap(deme_evolve, self.population, 1)# deme_evolve(self.population[0])#
    #            result=[]
    #            for i in range(len(self.population)):
    #                result.append(deme_evolve(self.population[i]))
    #            iter_result = iter(result)
    #            
                tmp_populations = []
    
    #            first_population = iter_result.next()
    #            tmp_populations.append(first_population)
                
                for p in range(0, self.num_demes):
                    tmp_populations.append(iter_result.next())
                    
                    to_del = self.population[p]
                    self.population[p] = copy.deepcopy(tmp_populations[p])
                    del to_del
                
                '''
                Configuración de adyacencia en anillo. Los últimos de la primera población son remplazados por 
                los primeros de la última.
                '''
                if Util.random_flip_coin(Parameters.p_migration):
                    self.population[self.num_demes - 1].internal_pop[for_replace_loosers:] = copy.deepcopy(self.population[0].internal_pop[0:for_replace_migration])
                    
                for p in range(1, self.num_demes):
                    '''Migración:los últimos de la población actual = los primeros de la poblacion anterior -- Según cierta probabilidad p_migration'''
                    if Util.random_flip_coin(Parameters.p_migration):
                        self.population[p-1].internal_pop[for_replace_loosers:] = copy.deepcopy(self.population[p].internal_pop[0:for_replace_migration])
                
                
                stats = self.generation % freq_stats
                
                if stats == 0:
                    print "\n==================================================\n"
                    print "Generación " + str(self.generation) + ": "
                    print "\n==================================================\n"
                    self.best_individual_in_training()
                
        
        except Exception as e:
            print "EXCEPCION en generación " + str(self.generation)
            print e
            raise e
        
        
    def best_individual_in_training(self):
        deme_best = []
        #nro.demes/nro.procesadores = chunk_zise

        iter_result = self.pool.imap(Population.best_training_in_pop, self.population, Parameters.chunk_size_step)
        print "Los " + str(self.num_demes) + " mejores en Entrenamiento..."
        for deme in range(self.num_demes):
            best = iter_result.next()
            deme_best.append(best)
            print "--- Deme " + str(deme) + ", individuo " + str(best.index) + " " + str(best.error)
            print best
            print best.get_program_in_python()
            
        deme_best.sort(cmp=Individual.compare, reverse=True)
        
        return deme_best#[0]
    
    def best_individual_in_validation(self):
        deme_best = []
        
        iter_result = self.pool.imap(Population.best_validation_in_pop, self.population, 2)
        
        print "Los  " + str(self.num_demes) + " mejores en Validación..."
        for deme in range(self.num_demes):
            best = iter_result.next()
            deme_best.append(best)
            print "--- Deme " + str(deme) + ", individuo " + str(best.index) + " " + str(best.validation_error)
            print best
            print best.get_program_in_python()
            
        deme_best.sort(cmp=Individual.compare_validation_error)
        
        return deme_best[0]
        
        
if __name__ == "__main__":    
    m = MdpProcess()
    m.process()
    
    """t_inicio = time.time()
    pool = Pool(processes=Parameters.num_processors)
    '''
    Se crea el direcctorio de resultados si no existe'
    '''
    
#    diff = str(datetime.now())
#    folder = "../resultados/"
#    folder += diff[:19].replace(':', '.')
    
#    if not os.path.exists(folder):
#        os.makedirs(folder)
#    
    folder = "./"#sys.argv[1]
    '''
    Se escribe en un archivo los parámetros usados
    '''
    f_parameters = folder + "/parameters.txt"
    Util.parameters_to_file(f_parameters)
    
    
    positions = []

    for i in range(Parameters.n):
        if Parameters.config[i] == '0':
            positions.append(i)
            
    #positions = [15,23,39]    
    for i in positions:
        try:
            best_individuals = []
            t1 = time.time()
            print "\n************************************************************************************\n"
            print "---- Transformador " + str(i)
            print "\n************************************************************************************\n"
            
            ga = LGP(config_position=i, pool=pool, demes=Parameters.demes)
            ga.set_num_generations(Parameters.num_generations)
            ga.set_population_size(Parameters.population_size)
            ga.evolve(freq_stats=Parameters.freq_stats)
            
            #best_training = ga.best_individual_in_training()
            best_individuals = ga.best_individual_in_training()
            #best_validation = ga.best_individual_in_validation()
            #ga.terminate()
            
            #best_individuals.append(best_training)
            #best_individuals.append(best_validation)
            
            t2 = time.time()
            print "\n"
            print '%s Duracion %0.5f s' % ("Transf. " + str(i), (t2-t1))
            print "\n"
            
            iter_result = pool.imap(Individual.eval_individual, best_individuals, Parameters.chunk_size_step)
            #iter_result = Individual.eval_individual(best_individuals[0])
            
            final_table = []
            '''
            final_table[0] errores de validación con el mejor individuo de entrenamiento
            final_table[1] errorer de validación con el mejor individuo con datos de validación
            '''
            for j in range(len(best_individuals)):
                errors_j = iter_result.next()#iter_result
                errors_and_sum = Util.sum_errors(errors_j)
                errors_and_sum.append(i)
                final_table.append(errors_and_sum)
                
            '''
            Se escriben a archivos los errores y los mejores programas para cada transformador
            '''
            f_errors = folder + "/errores-TRAF" + str(i) + "-G" + str(ga.generation - 1) + ".csv"
            Util.errors_to_file(f_errors, final_table)
            f_programs = folder + "/programas-TRAF" + str(i) + "-G" + str(ga.generation - 1) + ".txt"
            Util.programs_to_file(f_programs, best_individuals)
            
        except KeyboardInterrupt:
            best_individuals = ga.best_individual_in_training()
            for j in range(len(best_individuals)):
                errors_j = iter_result.next()#iter_result
                errors_and_sum = Util.sum_errors(errors_j)
                errors_and_sum.append(i)
                final_table.append(errors_and_sum)
                

            '''
            Se escriben a archivos los errores y los mejores programas para cada transformador
            '''
            f_errors = folder + "/errores-TRAF" + str(i) + "-G" + str(ga.generation - 1) + ".csv"
            Util.errors_to_file(f_errors, final_table)
            f_programs = folder + "/programas-TRAF" + str(i) + "-G" + str(ga.generation - 1) + ".txt"
            Util.programs_to_file(f_programs, best_individuals)
            
            end = False
            while True: 
                q = raw_input("Continuar con los otros tranformadores? [y/N]") 
                
                if q in ('n', 'N'):
                    print "Terminando Workers..."
                    pool.close()
                    pool.join()
                    end = True
                    break
                elif q in ('y', 'Y'): 
                    print("Continuando...") 
                    break
            if end:
                break
            
            
    pool.close()
    pool.join()
    
    t_final = time.time()
    print '%s Duracion %0.5f s' % ("LGPMAIN", (t_final - t_inicio))
    print "\n"
#    raw_input()
"""