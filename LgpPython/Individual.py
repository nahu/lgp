# -*- coding: utf-8 -*-

"""
Módulo que define la clase del Individuo dentro del LGP
y las funciones a la clase

@authors:
- U{Nahuel Hernández<mailto:jnahuelhernandez@gmail.com>}
- U{Vanessa Cañete<mailto:vanessa.can.89@gmail.com>}

@since: 1.0
"""

import random
import copy
from Parameters import *
from Util import randomFlipCoin

def ini_individual(obj):
    obj.height = random.randint(num_min_instructions, num_ini_instructions)
    obj.genomeList = []
    obj.fitness = 0.0      
    
    for i in range(obj.height):
        instruction = []
        instruction.append(random.randint(op_min, op_max)) #Instrucciones
        instruction.append(random.randint(var_min, var_max))  #Registros destinos - Solo los variables
        instruction.append(random.randint(var_min, var_max)) #Solo puede ser variable.
        
        #operador 2 es constante con probabilidad p_const
        if randomFlipCoin(p_reg_op2_const):
            instruction.append(random.randint(cons_al_min, cons_in_max))
        else:
            instruction.append(random.randint(var_min, var_max))
        
        obj.genomeList.append(instruction)
            
    """Asegurar que el la ultima instrucción tenga como registro destino al registro de salida"""
    obj.genomeList[len(obj.genomeList)-1][1] = reg_out
    #obj.init_registers()
    
    obj.r_all = []
    obj.r_all += r_out
    obj.r_all += r_var
    
    [obj.r_all.append(random.uniform(0, const_max)) for i in range(cons_al_min, cons_al_max + 1)]
    return obj

def get_random_instruction():
    instruction = []
    instruction.append(random.randint(op_min, op_max)) #Instrucciones
    instruction.append(random.randint(var_min, var_max))  #Registros destinos - Solo los variables
    instruction.append(random.randint(var_min, var_max)) #Solo puede ser variable.
    
    #operador 2 es constante con probabilidad p_const
    if randomFlipCoin(p_reg_op2_const):
        instruction.append(random.randint(cons_al_min, cons_in_max))
    else:
        instruction.append(random.randint(var_min, var_max))
    
    return instruction
class Individual():
    """
        heigth: cantidad de instrucciones para un programa
        width: componentes de una instruccion [instrucción, destino, operador1, operador2, efectiva]
        reg_sal                      -> registro de salida
        op_min, op_max               -> instrucciones
        cons_in_min, cons_in_max     -> registros de entrada constantes --> estan en r_const
        cons_al_min, cons_al_max     -> Constantes aleatorias inicializadas entre (0,random_max)
        var_min, var_max             -> registros variables inicializados a 1.
    """
    def __init__(self, width, index):
        self.width = width
        self.genomeList = []
        self.fitness= 0.0
        self.index = index
        self.evaluated = False
        
            
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
    
        eff_i.reverse()     
        return eff_i
    
    
    def get_effective_registers(self, position):
        """
        intructions[i][0] = identificador de instucción
        intructions[i][1] = registro destino
        intructions[i][2] = registro operando 1
        intructions[i][3] = registro operando 2
        intructions[i][4] = efectiva o no efectiva
        """
        reg_eff = set([0])
        current_pos = len(self.genomeList) -1 
        for i in reversed(self.genomeList):
            if (i[1] in reg_eff):
                # los operadores unarios tiene identificador del 6 al 9
                reg_eff.remove(i[1])
                
                if (i[0] < 6):
                    reg_eff.add(i[2])
                
                reg_eff.add(i[3])
                if current_pos == position:
                    return list(reg_eff)
                current_pos-=1
        return list(reg_eff)
        
    """
    Funcion de evaluación de fitnes llamada desde la GPopulation.evaluate()
    """
    def eval_fitness(self):
        eff_instructions = self.get_effective_instructions(self.genomeList)
        
        program = ""
        for i in eff_instructions:
            if (i[3] >= register_offset):
                program += (operations[i[0]<<4].format(i[1], i[2], i[3] - register_offset) + '\n')
            else:
                program += (operations[i[0]].format(i[1], i[2], i[3]) + '\n')
                
        
        #in_t tiene las mediciones en el instante t
        error_a_quad = 0
    
        for t in range(0, training_lines -1):
            in_t = r_const[t]
            r_all = copy.copy(self.r_all)
            print "program"
            print program
            print "in_t"
            print in_t
            print "t"
            print t
            '''
            print "r_all"
            
            for i in range (0, len(r_all)):
                if (i< len(in_t)):
                    print i, r_all[i], in_t[i]
                else:
                    print i, r_all[i]
            '''
            #try:
            exec program
            error_a_quad += (r_all[0] - data_samples[t][index_to_predict]) ** 2
            #except:
            #    error_a_quad += (99) ** 2
            
        error_prom_quad = error_a_quad / training_lines
        
        self.fitness = 1 / error_prom_quad
        self.evaluated = True
        
        
    def evaluate(self):
        if not self.evaluated:
            self.eval_fitness()
        return self.fitness
    
    def __repr__(self):
        """ Return a string representation of Genome """
        ret = "- Individual\n"
        ret += "\tList size:\t %s\n" % (len(self.genomeList))
        ret += "\tList:\n"
        for line in self.genomeList:
            ret += "\t\t\t"
            for item in line:
                ret += "[%s] " % (item)
            ret += "\n"
        ret+= "\tFitness:\t\t %.6f\n\n" % (self.fitness,)

        ret += "\n"
        return ret    
    
    
    def clone(self):
        newcopy = copy.deepcopy(self)
        return newcopy    
        
if __name__ == "__main__":
    r = Individual(0, 4)
    r.init_class()
    r.initialize()

    print r.genomeList
    print r.r_all