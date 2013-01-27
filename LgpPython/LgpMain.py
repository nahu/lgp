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
    


if __name__ == "__main__":
    data_samples = read_samples()
    r_const = init_reg_in_const()

    genome = Individual(4) #instr. iniciales, 4 componentes por instruccion

    
    ga = GSimpleGA.GSimpleGA(genome)
    ga.setGenerations(100)
    ga.setMinimax(Consts.minimaxType["minimize"])
    ga.setCrossoverRate(1.0)
    ga.setMutationRate(0.03)
    ga.setPopulationSize(100)
    ga.initialize()
    '''
    print "len" + str(len (ga.internalPop))
    for i in range(0, len(ga.internalPop)):
        print i, ga.internalPop[i].r_all
    print ga.internalPop[0].genomeList
    '''
    print "r_const"
    print r_const
    ga.evolve(freq_stats=10)
    """ Dentro de ga.internalPop estan los individuos.
    Los individuos de la poblacion se inicializan cuando la poblacion se inicializa.
    ver en GSimpleGA.initialize """
    
    
    best = ga.bestIndividual()
