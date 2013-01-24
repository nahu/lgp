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
from Functions import r_const

import random


def fitness_func(obj, **args):
    return random.random()

def iniInd(obj, **args):
    for instruction in obj.genomeList:
        instruction[0] = random.randint(Individual.op_min, Individual.op_max) #Instrucciones
        instruction[1] = random.randint(Individual.var_min, Individual.var_max)  #Registros destinos - Solo los variables
        instruction[2] = random.randint(Individual.var_min, Individual.var_max) #Solo puede ser variable.
        #operador 2 con probabilidad p_const
        p_const = random.random()
        if p_const>=0.5:
            instruction[3] = random.randint(Individual.var_min, Individual.var_max) 
        else:
            p_const2 = random.random()
            if p_const2>=0.5:
                instruction[3] = random.randint(Individual.cons_in_min, Individual.cons_in_max)
            else:
                instruction[3] = random.randint(Individual.cons_al_min, Individual.cons_al_max)
        #instruction[4] = 0 #false para efectiva
    """Asegurar que el la ultima instrucción tenga como registro destino al registro de salida"""
    obj.genomeList[len(obj.genomeList)-1][1]=0
    init_registers(obj)
    
def init_registers(obj):
    obj.r_all=[]
    [obj.r_all.append(random.uniform(0,const_max)) 
     for i in range(Individual.cons_al_min,Individual.cons_al_max)]
    [obj.r_all.append(1.0) 
     for i in range(Individual.var_min,Individual.var_max)]



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
        Individual.r_const = data
        #Funciones sobre escritas
        self.initializator.set(iniInd)
        self.evaluator.set(fitness_func)

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
    
    def eval_func(self, genome):
        """ The evaluation function """
        return 1
        
if __name__ == "__main__":
    r = Individual(10,5)
    k=10
    r.init_class(0,1,1,9,k, k+1, k*2, k*2+1,k*4,[[]])
    r.initialize()
    print r.genomeList
    print r.r_all