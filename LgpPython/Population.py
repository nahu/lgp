# -*- coding: utf-8 -*-

"""
Módulo que define la clase de la Población dentro del LGP
y las funciones relacionadas

@authors:
- U{Nahuel Hernández<mailto:jnahuelhernandez@gmail.com>}
- U{Vanessa Cañete<mailto:vanessa.can.89@gmail.com>}

@since: 1.0
"""

from multiprocessing import Pool
from Individual import Individual, ini_individual, create_new_instruction, get_random_register
from Parameters import num_processors, chunk_size, pool_size,p_ins, p_del, num_max_instructions, num_min_instructions,\
op_min,op_max, step_size_const, p_regmut, p_opermut, p_constmut, var_min, var_max
from Util import list_swap_element, random_flip_coin

import random


def tournament(competitors):
    choosen = competitors[0]
    
    for i in range(1, len(competitors)):
        if choosen.evaluate() < i.evaluate():
            choosen = i
            
    return choosen


def crossover(mom, dad):
    sister = None
    brother = None

    cuts_points_mom = [random.randint(1, len(mom.genomeList)-1), random.randint(1, len(mom.genomeList)-1)]
    if cuts_points_mom[0] > cuts_points_mom[1]:
        list_swap_element(cuts_points_mom, 0, 1)
        
    cuts_points_dad = [random.randint(1, len(dad.genomeList)-1), random.randint(1, len(dad.genomeList)-1)]    
    if cuts_points_dad[0] > cuts_points_dad[1]:
        list_swap_element(cuts_points_dad, 0, 1)
    
    sister = mom.clone()
    sister.genomeList[cuts_points_mom[0]:cuts_points_mom[1]] = dad.genomeList[cuts_points_dad[0]:cuts_points_dad[1]]

    brother = dad.clone()
    brother.genomeList[cuts_points_dad[0]:cuts_points_dad[1]] = mom.genomeList[cuts_points_mom[0]:cuts_points_mom[1]]

    return (sister, brother)


def impresiones_mutacion(ins, mutpoint, eff):
    if ins:
        print "Insercion de instrucciones en el punto " + str(mutpoint)
        print "Registros efectivos al punto de mutacion"
        print eff
    else:
        print "Borrado de instrucciones en el punto " + str(mutpoint)


def macro_mutation(genome):
    """Agrega o quita instrucciones
     -- Alg. 6.1 -- 
     p_ins > p_del 
     """
    insertion = random_flip_coin(p_ins)
    mutation_point = random.randint(0, len(genome.genomeList) - 1)
        
    if len(genome.genomeList) < num_max_instructions and \
    (insertion or len(genome.genomeList) == num_min_instructions):
        new_instruction = create_new_instruction()
        reg_eff, to_mutate = genome.get_effective_registers(mutation_point)
        #impresiones_mutacion(insertion, mutation_point, reg_eff)
        
        while not reg_eff: 
            """
            se da en el caso de que el punto de mutación esté por debajo de la última
            instrucción efectiva 
            y ésta sea unaria con un operando constante
            """
            #cambiar el registro constante del operador unario por uno variable
            genome.genomeList[to_mutate][3] = random.randint(var_min, var_max)
            reg_eff, to_mutate = genome.get_effective_registers(mutation_point)
        
        new_instruction[1] = random.choice(reg_eff)
        genome.genomeList.insert(mutation_point, new_instruction)
        self.height += 1
        
        
    if len(genome.genomeList) > num_min_instructions and \
    (not insertion or len(genome.genomeList) == num_max_instructions):
        del genome.genomeList[mutation_point]
        #impresiones_mutacion(insertion,mutation_point,[])
    
    return genome


def micro_mutation(genome):
    """Muta las instrucciones efectivas internamente
    p_regmut = 0.5 #probabilidad de mutar un registro 
    p_opermut = 0.25 #probabilidad de mutar una
    p_constmut = 0.25
    """
    eff, indices = genome.get_effective_instructions_with_indices()
    index = random.randint(0, len(eff) - 1)
    #print "index " + str(index)
    instruction = eff[index]
    mutation_point = indices[index]
    #print "mutation point: " + str(mutation_point) + " index : " + str(index)
    
    type = select_micro_mutacion_type(random.random())

    if (type == "constantes"):
        constants_indices = genome.get_effective_constant_indices()
        print constants_indices
        if constants_indices:
            ins_with_constant_index = random.choice(constants_indices)
            register_mutation_index = genome.genomeList[ins_with_constant_index][3]
            genome.r_all[register_mutation_index] += ((-1)**(random.randint(0, 1)) * random.uniform(0, step_size_const))
            
        else: #no hay instrucciones efectivas con registros constantes variables
            type = select_micro_mutacion_type(random.random()) #si vuelve a salir constante, se elige mutación de registro o operaciones con igual probabilidad
            if (type == "constantes"):
                type = "registros" if random_flip_coin(0.5) else "operaciones"
                
                
    if (type == "registros"):
        if len(eff) == 1: #una sola instrucción efectiva, no se puede cambiar r[0]
            if (instruction[0] < 5):
                pos_to_replace = random.randint(2, 3)
            else: #operación unaria, cambiar el segundo operando
                pos_to_replace = 3
        else:
            if (instruction[0] < 5):
                pos_to_replace = random.randint(1, 3)
            else: #operación unaria, cambiar el segundo operando o el destino
                pos_to_replace = 1 if random_flip_coin(0.5) else 3
        
        if pos_to_replace == 1:

            if (index + 1) < len(indices):
                reg_eff, to_mutate = genome.get_effective_registers(indices[index + 1])
                
                if reg_eff:
                    reg_eff.remove(instruction[1]) #remover el registro destino de los registros efectivos
                    
                op, pos_to_replace = get_random_register(pos_to_replace, reg_eff, instruction)
            else: #el punto de mutación es la última instrucción con el r[0]
                if (instruction[0] < 5): 
                    pos_to_replace = random.randint(2, 3)
                else: #operación unaria, cambiar el segundo operando
                    pos_to_replace = 3
                    
                op, pos_to_replace = get_random_register(pos_to_replace)
        else: #para los casos de operandos 1 y 2
            op, pos_to_replace = get_random_register(pos_to_replace)
            
        instruction[pos_to_replace] = op
            
        genome.genomeList[mutation_point] = instruction
        
    if (type == "operaciones"):
        diff_op = random.randint(op_min, op_max)
        
        while instruction[0] == diff_op:
            diff_op = random.randint(op_min, op_max)
            
        instruction[0] = diff_op
        genome.genomeList[mutation_point] = instruction
    
    return genome

def select_micro_mutacion_type(prob):
    if prob <= p_regmut:
        return "registros"
    elif prob > p_regmut and prob <= p_opermut + p_regmut:
        return "operaciones"
    elif prob > (p_opermut + p_regmut) and prob <= (p_opermut + p_regmut + p_constmut):
        return "constantes"



class Population:
    def __init__(self, size, pool):
        self.internalPop = []
        self.popSize = size
        self.pool = pool

    def create(self, **args):
        for i in xrange(self.popSize):
            self.internalPop.append(Individual(4, i))

    def initialize(self):
        self.create()
        
        iter = self.pool.imap(ini_individual, self.internalPop, chunk_size)

        for individual in range(self.popSize):
            self.internalPop[individual] = iter.next()
       
#        self.pool.close() 
#        self.pool.join()
        
        
    def selection(self):
        index_pool = []
        
        while len(index_pool) < pool_size:
            index = random.randint(0,len(population.internalPop))
            if not (self.internalPop[index] in index_pool):
                index_pool.append(self.internalPop[index])
                
        return index_pool
    
    
if __name__ == "__main__":
    pool = Pool(processes=num_processors)
    population = Population(2, pool)
    population.initialize()
    genome = population.internalPop[0]
    print "Instruciones originales"
    print genome
    print "Effectivas antes"
    print genome.get_program_in_python()
    genome1 = micro_mutation(genome)
    print "Instruciones mutadas"
    print genome1
    
    print "Effectivas después"
    print genome.get_program_in_python()
    
    
    pool.close() 
    pool.join()
    