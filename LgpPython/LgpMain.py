# -*- coding: utf-8 -*-

"""
Módulo que define el algoritmo LGP

@authors:
- U{Nahuel Hernández<mailto:jnahuelhernandez@gmail.com>}
- U{Vanessa Cañete<mailto:vanessa.can.89@gmail.com>}

@since: 1.0
"""

from sets import Set
from multiprocessing import Pool
from Population import Population, tournament

import Util
from Parameters import *

import math
import copy
import random



"""
if __name__ == '__main__':
    pool = Pool(processes=num_processors)
    
    '''
    imap aplica la función eval_fitness para cada elemento de la lista population,
    es decir, a cada individuo de la población
    
    si tarda mucho, considerar usar map o imap_unordered...
    '''
    iter = pool.imap(eval_fitness, population, chunk_size)
    
    #ver bien como se llaman los atributos de individual
    for individual in population:
        individual.fitness = iter.next()
    
    pool.close()
    pool.join()
"""
#INICIALIZACION DE REGISTROS DE ENTRADA CONSTANTES
def init_reg_in_const():
    """
    Una solo lista para los registros de entrada constantes. Todos los individuos lo usan.
    
    r_const lista de instantes, cada instante t tiene las medidas Xi de los transformadores en donde hay un medidor
    
    X[t][i] es el entero leido del archivo, medición del transformador i en el instante t
    """
    #Para cada t en el periodo de entrenamiento
    const = []

    for t in range(0, training_lines):
        instant = []
        for i in range(0, n):
            if (config[i] == '1'):
                instant.append(data_samples[t][i])
            
        const.append(instant)
        
    return const
    

def read_samples():
    n_data, lines_data, data = Util.get_matrix_from_file(filename)
    
    if ( n_data != n or lines_data != lines):
        print "El archivo no coincide con los parámetros"
        exit(-1)
        
    return data
    

class LPG():
    def __init__(self, population_size=1, generations=10):
        self.population = Population(population_size)
        self.num_generations = generations
        self.generation = 0
        
        self.pool = Pool(processes=num_processors)
        
        
    def terminate(self):
        self.pool.close() 
        self.pool.join()
        
        
    def set_population_size(self, population_size):
        self.population = Population(population_size)
        
        
    def set_num_generations(self, generations):
        self.num_generations = generations
        
        
    def step(self):
        to_tour_1 = self.population.selection()
        to_tour_2 = self.population.selection()
        
        iter = self.pool.imap(tournament, [to_tour_1, to_tour_2], 1)
        
        
        
        for individual in range(self.popSize):
            self.internalPop[individual] = iter.next()
        
        
    def termination_criteria(self):
        return self.generation == self.num_generations
    
    
    def evolve(self, freq_stats=0):
        
        self.population.initialize()
        self.generation = 0
        
        try:
            while not self.termination_criteria():
                self.generation += 1
                self.step()
                stats = self.generation % freq_stats if freq_stats > 0 else 1
                
                if stats == 0:
                    print "Generación " + str(self.generation) + " ...."
                
        except KeyboardInterrupt:
            print "Terminando Workers..."
            self.terminate()
            exit()
        
        
    def best_individual(self):
        pass
    
    
    def get_validation_errors(self, individual, data):
        pass
        
    
if __name__ == "__main__":

    data_samples = read_samples()
    r_const = init_reg_in_const()

    
    ga = LGP(100, 1000)
    ga.set_num_generations(1000)
    ga.set_population_size(100)

    '''
    print "len" + str(len (ga.internalPop))
    for i in range(0, len(ga.internalPop)):
        print i, ga.internalPop[i].r_all
    print ga.internalPop[0].genomeList
    '''

    ga.evolve(freq_stats=10)
    
    best = ga.best_individual()
    print "Solución"
    print best
