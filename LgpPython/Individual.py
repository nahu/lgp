# -*- coding: utf-8 -*-

"""
Módulo que define la clase del Individuo dentro del LGP
y las funciones a la clase

@authors:
- U{Nahuel Hernández<mailto:jnahuelhernandez@gmail.com>}
- U{Vanessa Cañete<mailto:vanessa.can.89@gmail.com>}

@since: 1.0
"""

from pyevolve import G2DList
from pyevolve import GSimpleGA
from pyevolve import Selectors
from pyevolve import Crossovers
from pyevolve import Mutators

from Functions import ini_individual, eval_fitness



class Individual(G2DList.G2DList):
    """
        heigth: cantidad de instrucciones para un programa
        width: componentes de una instruccion [instrucción, destino, operador1, operador2, efectiva]
        reg_sal                      -> registro de salida
        op_min, op_max               -> instrucciones
        cons_in_min, cons_in_max     -> registros de entrada constantes --> estan en r_const
        cons_al_min, cons_al_max     -> Constantes aleatorias inicializadas entre (0,random_max)
        var_min, var_max             -> registros variables inicializados a 1.
    """

    def init_class(self):
        """ Las siguientes funciones son sobre escritas en Functions.py"""
        self.initializator.set(ini_individual)
        self.evaluator.set(eval_fitness)
        
    """
    def init_registers(self):
        self.r_all = []
        self.r_all += r_out
        self.r_all += r_var
        
        [self.r_all.append(random.uniform(0, const_max)) for i in range(cons_al_min, cons_al_max)]
        
    """
        
    
    
        
if __name__ == "__main__":
    r = Individual(0, 4)
    r.init_class()
    r.initialize()

    print r.genomeList
    print r.r_all