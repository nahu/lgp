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

from Parameters import *
import random



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

    def init_class(self, reg_sal,op_min, op_max, cons_in_min, cons_in_max,
                     cons_al_min, cons_al_max, var_min, var_max, data):
        self.setRanges( reg_sal,op_min, op_max, cons_in_min, cons_in_max,cons_al_min, cons_al_max, var_min, var_max)
        self.init_registers()
        r_const = data
        """ Las siguientes funciones son sobre escritas en Functions.py"""
        self.initializator.set(ini_individual)
        self.evaluator.set(eval_fitness)
        
    def init_registers(self):
        self.r_all = []
        self.r_all += r_out
        self.r_all += r_var
        
        [self.r_all.append(random.uniform(0, const_max)) for i in range(cons_al_min, cons_al_max)]


    def setRanges(self, reg_sal,op_min, op_max, cons_in_min, 
                  cons_in_max,cons_al_min, cons_al_max, var_min, var_max):
        Individual.reg_sal = reg_sal
        Individual.op_min = op_min
        Individual.op_max = op_max        
        Individual.cons_in_min = cons_in_min
        Individual.cons_in_max = cons_in_max
        Individual.cons_al_min = cons_al_min
        Individual.cons_al_max = cons_al_max
        Individual.var_min = var_min
        Individual.var_max = var_max
        
        
    def get_effective_instructions(self):
        """
        intructions[i][0] = identificador de instucción
        intructions[i][1] = registro destino
        intructions[i][2] = registro operando 1
        intructions[i][3] = registro operando 2
        intructions[i][4] = efectiva o no efectiva
        """
        reg_eff = set([0])
        eff_i = []
        for i in reversed(self.genomeList):
            if (i[1] in reg_eff):
                # los operadores unarios tiene identificador del 6 al 9
                reg_eff.remove(i[1])
                
                if (i[0] < 6):
                    reg_eff.add(i[2])
                
                reg_eff.add(i[3])
                eff_i.append(i)
                
        return eff_i.reverse()
    
        
if __name__ == "__main__":
    r = Individual(10,4)
    k=10
    r.init_class(0,1,1,9,k, k+1, k*2, k*2+1,k*4,[[]])
    r.initialize()
    print r.genomeList
    print r.r_all