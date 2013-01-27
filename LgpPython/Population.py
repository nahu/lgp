# -*- coding: utf-8 -*-

from multiprocessing import Pool
from Parameters import num_processors, chunk_size, pool_size,p_ins, p_del, num_max_instructions, num_min_instructions
from Individual import Individual, ini_individual, get_random_instruction
from Util import list_swap_element, randomFlipCoin

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


def micro_mutation(genome):
    """Muta las instrucciones efectivas internamente"""


def macro_mutation(genome):
    """Agrega o quita instrucciones - Alg. 6.1 --p_ins > p_del """
    insertion = randomFlipCoin(p_ins)
    mutation_point = random.randint(0, len(genome.genomeList))    
    if len(genome.genomeList) < num_max_instructions and \
    (insertion or len(genome.genomeList) == num_min_instructions):
        new_instruction= get_random_instruction()
        eff = genome.get_effective_registers(mutation_point)
        new_instruction[1] = random.choice(eff)
        genome.genomeList[mutation_point] = new_instruction
        impresiones_mutacion(insertion,mutation_point,eff)
    if len(genome.genomeList) > num_min_instructions and \
    (not insertion or len(genome.genomeList) == num_max_instructions):
        del genome.genomeList[mutation_point]
        impresiones_mutacion(insertion,mutation_point,[])
    return genome

            


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
    genome1 = macro_mutation(genome)
    print "Instruciones mutadas"
    print genome1
    
    
    pool.close() 
    pool.join()
    