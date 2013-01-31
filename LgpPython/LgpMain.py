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
import Individual


class LGP():
    def __init__(self, population_size=1, generations=10, config_position=0):
        self.pool = Pool(processes=Parameters.num_processors)
        self.population = Population.Population(population_size, self.pool, config_position)
        self.num_generations = generations
        self.generation = 0
        self.config_position = config_position
    
    
    def terminate(self):
        self.pool.close() 
        self.pool.join()
        
        
    def set_population_size(self, population_size):
        self.population = Population.Population(population_size, self.pool, self.config_position)
        
        
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
        
        iter = self.pool.imap(Population.tournament, to_tournament, Parameters.chunk_size)

        winners = []
        for i in range(2):
            winners.append(iter.next())
        
        
        '''Se hacen copias temporales para remplazar luego a los perdedores del torneo'''
        winners_tmp = []
        for w in winners:
            winners_tmp.append(w.clone())
        
        if Util.random_flip_coin(Parameters.p_crossover):
            winners[0], winners[1] = Population.crossover(winners[0], winners[1])
            
        if not self.check_out_register(winners[0]) or not self.check_out_register(winners[1]):
            print "El crossover mató"
            
        for i in range(2):
            try:
                if Util.random_flip_coin(Parameters.p_macro_mutation):
                    winners[i] = Population.macro_mutation(winners[i])
            
                if not self.check_out_register(winners[i]):
                    print "La macro mató"
            except:
                print "La macro matOOOó"
                 
            if Util.random_flip_coin(Parameters.p_micro_mutation):
                winners[i] = Population.micro_mutation(winners[i])
            try:   
                if not self.check_out_register(winners[i]):
                    print "La miiicro mató"
            except:
                print "La miiicro mató"
                
            '''Se elimina de la lista de participantes del torneo al ganador, para remplazar a los perdedores'''
            to_tournament_indices[i].remove(winners[i].index)
            
            '''Se setean las copias temporales en las posiciones de los perdedores del torneo
            y se actualiza el indice dentro de la poblacion'''
            for l in to_tournament_indices[i]:
                self.population.internal_pop[l] = winners_tmp[i].clone()
                self.population.internal_pop[l].index = l
            
            '''Se remplaza a los ganadores del torneo por los nuevos individos operados geneticamente'''    
            self.population.internal_pop[winners[i].index] = winners[i]

        
    def termination_criteria(self):
        return self.generation == self.num_generations
    
    
    def check_out_register(self, individual):
        return individual.genomeList[individual.height - 1][1] == 0
    
    
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
#    print Parameters.data_samples
#    print Parameters.r_const
    best_individuals = []
    for i in range(Parameters.n):
        if Parameters.config[i] == '0':
            print "Transformador " + str(i)
            ga = LGP(config_position=i)
            ga.set_num_generations(Parameters.num_generations)
            ga.set_population_size(Parameters.population_size)
            ga.evolve(freq_stats=Parameters.freq_stats)
            
            best = ga.best_individual()
            ga.terminate()
            
            best_individuals.append(best)
        #    print "Solución"
        #    print best
            
    
    pool = Pool(processes=Parameters.num_processors)
    
    iter = pool.imap(Individual.eval_individual, best_individuals, Parameters.chunk_size)
    
    final_table = []
    for j in range(Parameters.n - Parameters.k):
        final_table.append(iter.next())
    f_errors = "errores" + "-G" + str(Parameters.num_generations) + "_P" + str(Parameters.population_size) + ".csv"
    f = open(f_errors, "w")
    for t in range(Parameters.validation_lines):
        row = ""
        for i in range(Parameters.n - Parameters.k):
            row += (str(final_table[i][t]) + ";")
        row += "\n"
        f.write(row.replace('.', ','))
    
    f_programs = "programas" + "-G" + str(Parameters.num_generations) + "_P" + str(Parameters.population_size) + ".txt"
    g = open(f_programs, "w")
    for b in best_individuals:
        g.write(repr(b))
        g.write("\n")
        g.write(b.get_program_in_python())
        g.write("\n\n\n")
        
        
    g.close()
    f.close()
    pool.close()
    pool.join()
