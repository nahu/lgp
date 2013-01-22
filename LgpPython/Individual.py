# -*- coding: utf-8 -*-

from pyevolve import G2DList
from pyevolve import GSimpleGA
from pyevolve import Selectors
from pyevolve import Crossovers
from pyevolve import Mutators
import random

# This function is the evaluation function, we want
# to give high score to more zero'ed chromosomes
def iniInd(obj, **args):
        for row in obj.genomeList:
            row[0] = random.randint(Individual.op_min, Individual.op_max) #Instrucciones
            row[1] = random.randint(Individual.var_min, Individual.var_max)  #Registros destinos - Solo los variables
            row[2] = random.randint(Individual.var_min, Individual.var_max) #Solo puede ser variable.
            #operador 2 con probabilidad p_const
            p_const = random.random()
            if p_const>=0.5:
                row[3] = random.randint(Individual.var_min, Individual.var_max) 
            else:
                p_const2 = random.random()
                if p_const2>=0.5:
                    row[3] = random.randint(Individual.cons_in_min, Individual.cons_in_max)
                else:
                    row[3] = random.randint(Individual.cons_al_min, Individual.cons_al_max)
            row[4] = 0 #false para efectiva
        """Asegurar que el la ultima instrucción tenga como registro destino al registro de salida"""
        obj.genomeList[len(obj.genomeList)-1][1]=0
      


class Individual(G2DList.G2DList):
    reg_sal = 0
    op_min = 0 
    op_max = 0
    cons_in_min = 0
    cons_in_max = 0
    cons_al_min = 0
    cons_al_max = 0
    var_min = 0
    var_max = 0
    const = 0
    """
        heigth: cantidad de instrucciones para un programa
        width: componentes de una instruccion [instrucción, destino, operador1, operador2, efectiva]
        reg_sal                      -> registro de salida
        op_min, op_max               -> instrucciones
        cons_in_min, cons_in_max     -> registros de entrada constantes --> estan en r_const
        cons_al_min, cons_al_max     -> Constantes aleatorias inicializadas entre (0,random_max)
        var_min, var_max             -> registros variables inicializados a 1.
    """

    def inicializar(self, reg_sal,op_min, op_max, cons_in_min, cons_in_max,
                     cons_al_min, cons_al_max, var_min, var_max):
        self.setRanges( reg_sal,op_min, op_max, cons_in_min, cons_in_max,cons_al_min, cons_al_max, var_min, var_max)
        self.initializator.set(iniInd)
#        self.initialize()
#        self.evaluator.set(self.eval_func)

    def setRanges(self, reg_sal,op_min, op_max, cons_in_min, cons_in_max,cons_al_min, cons_al_max, var_min, var_max):

        Individual.reg_sal = reg_sal
        Individual.op_min = op_min
        Individual.op_max = op_max        
        Individual.cons_in_min = cons_in_min
        Individual.cons_in_max = cons_in_max
        Individual.cons_al_min = cons_al_min
        Individual.cons_al_max = cons_al_max
        Individual.var_min = var_min
        Individual.var_max = var_max

    def initialize(self):
        for row in self.genomeList:
            row[0] = random.randint(Individual.op_min, Individual.op_max) #Instrucciones
            row[1] = random.randint(Individual.var_min, Individual.var_max)  #Registros destinos - Solo los variables
            row[2] = random.randint(Individual.var_min, Individual.var_max) #Solo puede ser variable.
            #operador 2 con probabilidad p_const
            p_const = random.random()
            if p_const>=0.5:
                row[3] = random.randint(Individual.var_min, Individual.var_max) 
            else:
                p_const2 = random.random()
                if p_const2>=0.5:
                    row[3] = random.randint(Individual.cons_in_min, Individual.cons_in_max)
                else:
                    row[3] = random.randint(Individual.cons_al_min, Individual.cons_al_max)
            row[4] = 0 #false para efectiva
        """Asegurar que el la ultima instrucción tenga como registro destino al registro de salida"""
        self.genomeList[len(self.genomeList)-1][1]=0
    
    def eval_func(self, genome):
        """ The evaluation function """
        return 1
        
if __name__ == "__main__":
    r = Individual(10,5)
    k=10
    r.inicializar(0,1,1,9,k, k+1, k*2, k*2+1,k*4)
    r.initialize()
    print Individual.const
    print r.genomeList