# -*- coding: utf-8 -*-

from multiprocessing import Pool
from Parameters import num_processors, chunk_size, pool_size
from Individual import Individual, ini_individual
from Util import list_swap_element

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

    #if args["count"] == 2:
    brother = dad.clone()
    brother.genomeList[cuts_points_dad[0]:cuts_points_dad[1]] = mom.genomeList[cuts_points_mom[0]:cuts_points_mom[1]]

    return (sister, brother)

class Population:
    def __init__(self, size):
        self.internalPop = []
        self.popSize = size

    def create(self, **args):
        for i in xrange(self.popSize):
            self.internalPop.append(Individual(4, i))

    def initialize(self):
        self.create()
        pool = Pool(processes=num_processors)
        
        iter = pool.imap(ini_individual, self.internalPop, chunk_size)

        for individual in range(self.popSize):
            self.internalPop[individual] = iter.next()
        
        pool.close() 
        pool.join()
        
        
    def selection(self):
        index_pool = []
        while len(index_pool) < pool_size:
            index = random.randint(0,len(population.internalPop))
            if not (self.internalPop[index] in index_pool):
                index_pool.append(self.internalPop[index])
                
        return index_pool
    
    
if __name__ == "__main__":
    population = Population(10)
    population.initialize()
    print "len"
    print len(population.internalPop)
    for i in population.internalPop:
        print i.genomeList
    