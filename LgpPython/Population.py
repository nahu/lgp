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
import Individual
import Parameters

from Util import list_swap_element, random_flip_coin

import random


def tournament(competitors):
    choosen = competitors[0]
    
    for i in range(1, len(competitors)):
        if choosen.evaluate() < competitors[i].evaluate():
            choosen = competitors[i]
            
    print "ganador del torneo: " + str(choosen.index)
    
    bandera = False
    imprimir = "competidores:  "
    for i in range(0, len(competitors)):
        imprimir += " " + str(competitors[i].index)
        if competitors[i].index == choosen.index:
            bandera = True
            
    print imprimir
    if not bandera:
        print "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAa"
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
    insertion = random_flip_coin(Parameters.p_ins)
    mutation_point = random.randint(0, len(genome.genomeList) - 1)
        
    if len(genome.genomeList) < Parameters.num_max_instructions and \
    (insertion or len(genome.genomeList) == Parameters.num_min_instructions):
        new_instruction = Individual.create_new_instruction()
        reg_eff, to_mutate = genome.get_effective_registers(mutation_point)
        #impresiones_mutacion(insertion, mutation_point, reg_eff)
        
        while not reg_eff: 
            """
            se da en el caso de que el punto de mutación esté por debajo de la última
            instrucción efectiva 
            y ésta sea unaria con un operando constante
            """
            #cambiar el registro constante del operador unario por uno variable
            genome.genomeList[to_mutate][3] = random.randint(Parameters.var_min, Parameters.var_max)
            reg_eff, to_mutate = genome.get_effective_registers(mutation_point)
        
        new_instruction[1] = random.choice(reg_eff)
        genome.genomeList.insert(mutation_point, new_instruction)
        genome.height += 1
        
        
    if len(genome.genomeList) > Parameters.num_min_instructions and \
    (not insertion or len(genome.genomeList) == Parameters.num_max_instructions):
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
            genome.r_all[register_mutation_index] += ((-1)**(random.randint(0, 1)) * random.uniform(0, Parameters.step_size_const))
            
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
                    
                op, pos_to_replace = Individual.get_random_register(pos_to_replace, reg_eff, instruction)
            else: #el punto de mutación es la última instrucción con el r[0]
                if (instruction[0] < 5): 
                    pos_to_replace = random.randint(2, 3)
                else: #operación unaria, cambiar el segundo operando
                    pos_to_replace = 3
                    
                op, pos_to_replace = Individual.get_random_register(pos_to_replace)
        else: #para los casos de operandos 1 y 2
            op, pos_to_replace = Individual.get_random_register(pos_to_replace)
            
        instruction[pos_to_replace] = op
            
        genome.genomeList[mutation_point] = instruction
        
    if (type == "operaciones"):
        diff_op = random.randint(Parameters.op_min, Parameters.op_max)
        
        while instruction[0] == diff_op:
            diff_op = random.randint(Parameters.op_min, Parameters.op_max)
            
        instruction[0] = diff_op
        genome.genomeList[mutation_point] = instruction
    
    return genome

def select_micro_mutacion_type(prob):
    if prob <= Parameters.p_regmut:
        return "registros"
    elif prob > Parameters.p_regmut and prob <= Parameters.p_opermut + Parameters.p_regmut:
        return "operaciones"
    elif prob > (Parameters.p_opermut + Parameters.p_regmut) and prob <= (Parameters.p_opermut + Parameters.p_regmut + Parameters.p_constmut):
        return "constantes"



class Population:
    def __init__(self, size, pool):
        self.internal_pop = []
        self.pop_size = size
        self.pool = pool

    def create(self, **args):
        for i in xrange(self.pop_size):
            self.internal_pop.append(Individual.Individual(4, i))

    def initialize(self):
        self.create()
        
        iter = self.pool.imap(Individual.ini_individual, self.internal_pop, Parameters.chunk_size)

        for individual in range(self.pop_size):
            self.internal_pop[individual] = iter.next()
       
#        self.pool.close() 
#        self.pool.join()
        
        
    def indices_selection(self, pool_size):
        index_pool = []
        
        while len(index_pool) < pool_size:
            index = random.randint(0, self.pop_size - 1)
            if not (index in index_pool):
                index_pool.append(index)
                
        return index_pool
    
    def selection_from_indices(self, indices):
        pool = []
        
        for i in indices:
            pool.append(self.internal_pop[i])
                
        return pool
    
    def best(self):
        best = self.internal_pop[0]
        
        for i in range(1, self.pop_size - 1):
            if (self.internal_pop[i].evaluate() > best):
                best = self.internal_pop[i]
        
        return best
    
    
    
if __name__ == "__main__":
    pool = Pool(processes=Parameters.num_processors)
    population = Population(2, pool)
    population.initialize()
    genome = population.internal_pop[0]
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
    