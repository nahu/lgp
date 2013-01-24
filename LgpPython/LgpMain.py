# -*- coding: utf-8 -*-

import Individual
from Reader import Reader
from Parameters import *
from pyevolve import GPopulation
from pyevolve import GSimpleGA
from pyevolve import Consts


class LgpMain():
    def __init__(self, config, filename):
        reader = Reader()
        config = config
        n_data, lines_data, data = reader.get_matrix_from_file(filename)
        
        if ( n_data != n or lines_data != lines):
            print "El archivo no coincide con los parámetros"
            exit(-1)
        
        r_const = init_reg_in_const()
        
    
    #INICIALIZACION DE REGISTROS DE ENTRADA CONSTANTES
    def init_reg_in_const(X):
        """
        Una solo lista para los registros de entrada constantes. Todos los individuos lo usan.
        
        r_const lista de instantes, cada instante t tiene las medidas Xi de los transformadores en donde hay un medidor
        
        X[t][i] es el entero leido del archivo, medición del transformador i en el instante t
        """
        #Para cada t en el periodo de entrenamiento
        for t in range(0, training_lines):
            instant = []
            for i in range(0, n):
                if (config[i] == '1'):
                    instant.append(data[t][i])
                
            r_const.append(instant)
    
    
if __name__ == "__main__":
    lgp = LgpMain(config, filename)    
    lgp.genome = Individual.Individual(0,4) #instr. iniciales, 5 componentes por instruccion
    lgp.genome.init_class(0,1,1,9,lgp.k, lgp.k+1, lgp.k*2, lgp.k*2+1,lgp.k*4, lgp.r_const)
    print lgp.genome.r_const
    """ se podria hacer lo siguiente"""
    ga = GSimpleGA.GSimpleGA(lgp.genome)
    ga.setGenerations(100)
    ga.setMinimax(Consts.minimaxType["minimize"])
    ga.setCrossoverRate(1.0)
    ga.setMutationRate(0.03)
    ga.setPopulationSize(10000)
    ga.initialize()
    print ga.internalPop[0].genomeList
    ga.evolve(freq_stats=10)
    """ Dentro de ga.internalPop estan los individuos.
    Los individuos de la poblacion se inicializan cuando la poblacion se inicializa.
    ver en GSimpleGA.initialize """
    
    
    best = ga.bestIndividual()
