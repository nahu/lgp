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
from pyevolve import Util

from Reader import Reader
from Parameters import *

import math
import copy
import random


from pyevolve import GPopulation
from pyevolve import GSimpleGA
from pyevolve import G2DList
from pyevolve import Consts

def ini_individual(obj, **args):
    obj.height = random.randint(num_min_instructions, num_ini_instructions)
    
    for i in range(obj.height):
        instruction = []
        instruction.append(random.randint(op_min, op_max)) #Instrucciones
        instruction.append(random.randint(var_min, var_max))  #Registros destinos - Solo los variables
        instruction.append(random.randint(var_min, var_max)) #Solo puede ser variable.
        
        #operador 2 es constante con probabilidad p_const
        if Util.randomFlipCoin(p_reg_op2_const):
            instruction.append(random.randint(cons_al_min, cons_in_max))
        else:
            instruction.append(random.randint(var_min, var_max))
        
        obj.genomeList.append(instruction)
            
    """Asegurar que el la ultima instrucción tenga como registro destino al registro de salida"""
    obj.genomeList[len(obj.genomeList)-1][1] = reg_out
    #obj.init_registers()
    
    obj.r_all = []
    obj.r_all += r_out
    obj.r_all += r_var
    
    [obj.r_all.append(random.uniform(0, const_max)) for i in range(cons_al_min, cons_al_max + 1)]
    #print "var" + str(len(r_var))

def get_effective_instructions(genomeList):
    """
    intructions[i][0] = identificador de instucción
    intructions[i][1] = registro destino
    intructions[i][2] = registro operando 1
    intructions[i][3] = registro operando 2
    intructions[i][4] = efectiva o no efectiva
    """
    reg_eff = set([0])
    eff_i = []
    for i in reversed(genomeList):
        if (i[1] in reg_eff):
            # los operadores unarios tiene identificador del 6 al 9
            reg_eff.remove(i[1])
            
            if (i[0] < 6):
                reg_eff.add(i[2])
            
            reg_eff.add(i[3])
            eff_i.append(i)

    eff_i.reverse()     
    return eff_i

"""
Funcion de evaluación de fitnes llamada desde la GPopulation.evaluate()
"""
def eval_fitness(obj, **args):
    eff_instructions = get_effective_instructions(obj.genomeList)
    print "eff"
    print eff_instructions
    print "obj"
    print obj
    program = ""
    for i in eff_instructions:
        if (i[3] >= register_offset):
            program += (operations[i[0]<<4].format(i[1], i[2], i[3] - register_offset) + '\n')
        else:
            program += (operations[i[0]].format(i[1], i[2], i[3]) + '\n')
            
    
    #in_t tiene las mediciones en el instante t
    error_a_quad = 0

    for t in range(0, training_lines -1):
        in_t = r_const[t]
        r_all = copy.copy(obj.r_all)
        print "program"
        print program
        print "in_t"
        print in_t
        print "t"
        print t
        '''
        print "r_all"
        
        for i in range (0, len(r_all)):
            if (i< len(in_t)):
                print i, r_all[i], in_t[i]
            else:
                print i, r_all[i]
        '''
        #try:
        exec program
        error_a_quad += (r_all[0] - data_samples[t][index_to_predict]) ** 2
        #except:
        #    error_a_quad += (99) ** 2
        
    error_prom_quad = error_a_quad / training_lines
    
    fitness = 1 / error_prom_quad
    
    return fitness

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
    reader = Reader()
    n_data, lines_data, data = reader.get_matrix_from_file(filename)
    
    if ( n_data != n or lines_data != lines):
        print "El archivo no coincide con los parámetros"
        exit(-1)
        
    return data
    


if __name__ == "__main__":
    data_samples = read_samples()
    r_const = init_reg_in_const()

    genome = G2DList.G2DList(0,4) #instr. iniciales, 4 componentes por instruccion
    genome.initializator.set(ini_individual)
    genome.evaluator.set(eval_fitness)
    genome.initialize()

    
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
