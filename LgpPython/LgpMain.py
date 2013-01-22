# -*- coding: utf-8 -*-
from Individual import Individual
from Reader import Reader
from pyevolve import GPopulation
from pyevolve import GSimpleGA
from pyevolve import Consts
class LgpMain():
    
    def init_parameters(self):
        #REGISTROS
        """
        r[0] registro de salida
        r[1] .. r[self.k] registros de entrada constantes
        r[self.k+1] .. r[2*self.k] registros aleatorios constantes
        r[2*self.k + 1] .. r[4*self.k] registros variables inicializados a 1
        """
        
        self.num_registers = 5*self.k + 1
        self.num_conts_registers = 2*self.k
        self.num_var_register = 2*self.k
        self.num_const_in_registers = self.k
        self.num_out_registers = 1
        self.num_const_random_registers = self.k
        
        #INSTRUCCIONES
        """
        Cada individuo posee una lista de instrucciones
        Cada instrucci�n se representa como una lista de 4 enteros y 1 booleano como sigue
        [instrucci�n, destino, operador1, operador2, efectiva]
        efectiva indica si la instrucci�n es efectiva o no, se inicializa a "false"
        la convenci�n para asegurar que el programa tenga una salida es que la primera 
        """
        self.num_min_instructions = self.k
        self.num_max_instructions = 6*self.k
        self.num_ini_instructions = 2*self.k
        self.num_operators = 9
        
        
        #ALGORITMO EVOLUTIVO
        self.num_generations = 10000
        self.population_size = 1000
        
        #PROBABILIDADES
        self.p_reg_op2_const = 0.5
        
        
        #DATOS
        self.training_lines = 144
        self.validation_lines = 96

    def __init__(self, config, filename):
        reader = Reader()
        self.n,self.T, self.data = reader.get_matrix_from_file(filename)        
        self.k = config.count('1')
        self.r_const = []
    def initialize_population(self):
        self.genome = Individual(lgp.num_ini_instructions,5) #instr. iniciales, 5 componentes por instruccion
        self.genome.inicializar(range(1,lgp.k), range(lgp.k+1, 2*lgp.k), range(2*lgp.k+1, lgp.k*4), range(1,9) )
        self.population = GPopulation.GPopulation(self.genome)
        self.population.setPopulationSize(self.population_size)
        self.population.create(minimax = Consts.minimaxType["maximize"])
        self.population.initialize()
        
if __name__ == "__main__":
    config = "1010101010" + "1010101010" + "1010101010" + "1010101010" 
    filename = "Datos60.txt"
    lgp = LgpMain(config,filename)
    lgp.init_parameters()
    lgp.initialize_population()
    
    print lgp.population.internalPop[0].genomeList
    
    """ se podria hacer lo siguiente"""
#    ga = GSimpleGA.GSimpleGA(lgp.genome)
#    ga.setGenerations(100)
#    ga.setMinimax(Consts.minimaxType["minimize"])
#    ga.setCrossoverRate(1.0)
#    ga.setMutationRate(0.03)
#    ga.setPopulationSize(80)
#    ga.evolve(freq_stats=10)
#    """ Dentro de ga.internalPop estan los individuos.
#    Los individuos de la poblacion se inicializan cuando la poblacion se inicializa.
#    ver en GSimpleGA.initialize """
#    print ga.internalPop[0].genomeList
#    
#    best = ga.bestIndividual()
