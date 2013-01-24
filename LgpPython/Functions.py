# -*- coding: utf-8 -*-
"""
Módulo que define las funciones asociadas al algoritmo LGP

@authors:
- U{Nahuel Hernández<mailto:jnahuelhernandez@gmail.com>}
- U{Vanessa Cañete<mailto:vanessa.can.89@gmail.com>}

@since: 1.0
"""

from sets import Set
from multiprocessing import Pool
from Parameters import *

import math
import copy


#INICIALIZACION DE REGISTROS DE ENTRADA CONSTANTES
"""
Una solo lista para los registros de entrada constantes.
Todos los individuos lo usan.

r_const lista de instantes, cada instante t tiene las medidas Xi
de los transformadores en donde hay un medidor


X[t][i] es el entero leido del archivo, 
medición del transformador i en el instante t
"""
r_const = []
def init_reg_in_const(X):
    #Para cada t en el periodo de entrenamiento
    for t in range(0, training_lines):
        instant = []
        for i in range(0, n):
            if (config[i] == '1'):
                instant.append(X[t][i])
            
        r_const.append(instant)
    


def mark_effective_instructions(instructions):
    """
    intructions[i][0] = identificador de instucción
    intructions[i][1] = registro destino
    intructions[i][2] = registro operando 1
    intructions[i][3] = registro operando 2
    intructions[i][4] = efectiva o no efectiva
    """
    reg_eff = set([0])
    eff_i = []
    for i in reversed(instructions):
        if (i[1] in reg_eff):
            # los operadores unarios tiene identificador del 6 al 9
            reg_eff.remove(i[1])
            
            if (i[0] < 6):
                reg_eff.add(i[2])
            
            reg_eff.add(i[3])
            #i[4] = True
            eff_i.append(i)
            
    return eff_i.reverse()



"""
se debe llamar com eval_fitness(individual.genomeList)
"""
def eval_fitness(individual):
    eff_instructions = mark_effective_instructions(individual.genomeList)
    
    program = ""
    for i in eff_instructions:
        if (i[3] >= register_offset):
            program += operations[i[0]<<4].format(i[1], i[2], i[3] - register_offset)
        else:
            program += operations[i[0]].format(i[1], i[2], i[3])
    
    #in_t tiene las mediciones en el isntante t
    error_a_quad = 0
    for t in range(0, training_lines):
        in_t = r_const[t]
        r_all = copy.copy(individual.r_all)
        exec program
        error_a_quad += (r_all[0] - X[t][index_to_predict]) ** 2
        
    fitness = 1/error_a_quad
    
    return fitness


if __name__ == '__main__':
    pool = Pool(processes=num_processors)
    
    """
    imap aplica la función eval_fitness para cada elemento de la lista population,
    es decir, a cada individuo de la población
    
    si tarda mucho, considerar usar map o imap_unordered...
    """
    iter = pool.imap(eval_fitness, population, chunk_size)
    
    #ver bien como se llaman los atributos de individual
    for individual in population:
        individual.fitness = iter.next()
    
    pool.close()
    pool.join()
    
    