# -*- coding: utf-8 -*-

"""
Módulo que define el algoritmo LGP

@authors:
- U{Nahuel Hernández<mailto:jnahuelhernandez@gmail.com>}
- U{Vanessa Cañete<mailto:vanessa.can.89@gmail.com>}

@since: 1.0
"""

from multiprocessing import Pool

import Population
import Util
import Parameters
import copy



class LGP():
    def __init__(self, population_size=1, generations=10):
        self.pool = Pool(processes=Parameters.num_processors)
        self.population = Population.Population(population_size, self.pool)
        self.num_generations = generations
        self.generation = 0
           
    def terminate(self):
        self.pool.close() 
        self.pool.join()
        
        
    def set_population_size(self, population_size):
        self.population = Population.Population(population_size, self.pool)
        
        
    def set_num_generations(self, generations):
        self.num_generations = generations
        
        
    def step(self):
        to_tournament = []
        selected_indices = self.population.indices_selection(Parameters.pool_size * 2)
        
        to_tournament_indices = []
        to_tournament_indices.append(selected_indices[:Parameters.pool_size])
        to_tournament_indices.append(selected_indices[Parameters.pool_size:])
        to_tournament.append(self.population.selection_from_indices(to_tournament_indices[0]))
        to_tournament.append(self.population.selection_from_indices(to_tournament_indices[1]))
        
        iter = self.pool.imap(Population.tournament, to_tournament, 99999)

        winners = []
        for i in range(2):
            winners.append(iter.next())
        
        print winners
        '''Se hacen copias temporales para remplazar luego a los perdedores del torneo'''
        winners_tmp = []
        for w in winners:
            winners_tmp.append(copy.deepcopy(w))
        
        if Util.random_flip_coin(Parameters.p_crossover):
            winners[0], winners[1] = Population.crossover(winners[0], winners[1])
        
        for i in range(2):
            if Util.random_flip_coin(Parameters.p_macro_mutation):
                winners[i] = Population.macro_mutation(winners[i])
                
            if Util.random_flip_coin(Parameters.p_micro_mutation):
                winners[i] = Population.micro_mutation(winners[i])
            '''Se elimina de la lista de participantes del torneo al ganador, para remplazar a los perdedores'''
            to_tournament_indices[i].remove(winners[i].index)
            
            '''Se setean las copias temporales en las posiciones de los perdedores del torneo
            y se actualiza el indice dentro de la poblacion'''
            for l in to_tournament_indices[i]:
                self.population.internal_pop[l] = copy.deepcopy(winners_tmp[i])
                self.population.internal_pop[l].index = l
            
            '''Se remplaza a los ganadores del torneo por los nuevos individos operados geneticamente'''    
            self.population.internal_pop[winners[i].index] = winners[i]

        
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
        return self.population.best()
        
    
    def get_validation_errors(self, individual, data):
        pass
        
    
if __name__ == "__main__":
    ga = LGP()
    ga.set_num_generations(10)
    ga.set_population_size(10)
    ga.evolve(freq_stats=1)
    
    best = ga.best_individual()
    print "Solución"
    print best