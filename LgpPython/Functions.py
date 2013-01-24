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
import Util

def ini_individual(obj, **args):
    obj.height = random.randint(num_min_instructions, num_ini_instructions)
    
    for i in range(obj.height):
        instruction = []
        instruction.append(random.randint(op_min, op_max)) #Instrucciones
        instruction.append(random.randint(var_min, var_max))  #Registros destinos - Solo los variables
        instruction.append(random.randint(var_min, var_max)) #Solo puede ser variable.
        
        #operador 2 es constante con probabilidad p_const
        if Util.randomFlipCoin(p_const):
            instruction.append(random.randint(cons_al_min, cons_in_max))
        else:
            instruction.append(random.randint(var_min, var_max))
        
        obj.genomeList.append(instruction)
            
    """Asegurar que el la ultima instrucción tenga como registro destino al registro de salida"""
    obj.genomeList[len(obj.genomeList)-1][1] = reg_out
    init_registers(obj)


"""
Funcion de evaluación de fitnes llamada desde la GPopulation.evaluate()
"""
def eval_fitness(obj, **args):
    eff_instructions = obj.get_effective_instructions()
    
    program = ""
    for i in eff_instructions:
        if (i[3] >= register_offset):
            program += operations[i[0]<<4].format(i[1], i[2], i[3] - register_offset)
        else:
            program += operations[i[0]].format(i[1], i[2], i[3])
    
    #in_t tiene las mediciones en el instante t
    error_a_quad = 0
    for t in range(0, training_lines):
        in_t = r_const[t]
        r_all = copy.copy(self.r_all)
        exec program
        error_a_quad += (r_all[0] - data[t][index_to_predict]) ** 2
        
    error_prom_quad = error_a_quad / training_lines
    
    fitness = 1 / error_prom_quad
    
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
    
    