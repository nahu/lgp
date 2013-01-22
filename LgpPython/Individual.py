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
            row[0] = random.choice(Individual.op_instrucciones) #Instrucciones
            row[1] = random.choice(Individual.op_reg_var) #Registros destinos - Solo los variables
            row[2] = random.choice(Individual.op_reg_var)#Solo puede ser variable.
            #operador 2 con probabilidad p_const
            prob = random.random()
            if prob>=0.5:
                row[3] = random.choice(Individual.op_reg_var)
            else:
                row[3] = random.choice(Individual.op_reg_in_const + Individual.op_reg_rand_const)
                Individual.const+=1
            row[4] = 0
        """Asegurar que el la ultima instrucción tenga como registro destino al registro de salida"""
        obj.genomeList[len(obj.genomeList)-1][1]=0
      

class Individual(G2DList.G2DList):
    op_reg_in_const = []
    op_reg_rand_const = []
    op_reg_var = []
    op_instrucciones = []
    const = 0
    """
        heigth: cantidad de instrucciones para un programa
        width: componentes de una instruccion [instrucción, destino, operador1, operador2, efectiva]
        op_reg_in_const: opciones de registros de entrada constantes
        op_reg_rand_const: opciones de registros aleatorios constantes
        op_reg_var: opciones de registros variables
        op_instrucciones: opciones de instrucciones
    """
    def inicializar(self, op_reg_in_const,op_reg_rand_const, op_reg_var, op_instrucciones):
        self.setRanges(op_reg_in_const, op_reg_rand_const, op_reg_var, op_instrucciones)
        self.initializator.set(iniInd)
        #self.initialize()
        self.evaluator.set(self.eval_func)

    def setRanges(self, op_reg_in_const, op_reg_rand_const, op_reg_var, op_instrucciones):
        """ Almacena los posibles valores para cada componente de la instruccion."""
        Individual.op_reg_in_const = op_reg_in_const
        Individual.op_reg_rand_const= op_reg_rand_const
        Individual.op_reg_var = op_reg_var
        Individual.op_instrucciones = op_instrucciones

    def initialize(self):
        for row in self.genomeList:
            row[0] = random.choice(Individual.op_instrucciones) #Instrucciones
            row[1] = random.choice(Individual.op_reg_var) #Registros destinos - Solo los variables
            row[2] = random.choice(Individual.op_reg_var)#Solo puede ser variable.
            prob = random.random()
            if prob>=0.5:
                row[3] = random.choice(Individual.op_reg_var)
            else:
                row[3] = random.choice(Individual.op_reg_in_const + Individual.op_reg_rand_const)
                Individual.const+=1
            row[4] = 0
        """Asegurar que el la ultima instrucción tenga como registro destino al registro de salida"""
        self.genomeList[len(self.genomeList)-1][1]=0
    
    def eval_func(self, genome):
        """ The evaluation function """
        return 1
        
if __name__ == "__main__":
    r = Individual(10,5)
    r.inicializar(range(1,9), range(1,4), range(1,4), range(1,20))
    r.initialize()
    print Individual.const
    print r.genomeList