# -*- coding: utf-8 -*-

"""
Módulo que define la clase del Individuo dentro del LGP
y las funciones relacionadas

@authors:
- U{Nahuel Hernández<mailto:jnahuelhernandez@gmail.com>}
- U{Vanessa Cañete<mailto:vanessa.can.89@gmail.com>}

@since: 1.0
"""

import random
import math
import copy
import copy

from Parameters import *
from Util import random_flip_coin

def ini_individual(obj):
    obj.height = random.randint(num_min_instructions, num_ini_instructions)
    obj.genomeList = []
    obj.fitness = 0.0      
    
    for i in range(obj.height):
        instruction = create_new_instruction()
        obj.genomeList.append(instruction)
            
    """Asegurar que la última instrucción tenga como registro destino al registro de salida"""
    obj.genomeList[len(obj.genomeList)-1][1] = reg_out
    #obj.init_registers()
    
    obj.r_all = []
    obj.r_all += r_out
    obj.r_all += r_var
    
    [obj.r_all.append(random.uniform(0, const_max)) for i in range(cons_al_min, cons_al_max + 1)]
    return obj


def create_new_instruction():
    """
    reg_sal                      -> registro de salida
    op_min, op_max               -> instrucciones
    cons_in_min, cons_in_max     -> registros de entrada constantes --> estan en r_const
    cons_al_min, cons_al_max     -> Constantes aleatorias inicializadas entre (0,random_max)
    var_min, var_max             -> registros variables inicializados a 1.
    """
    instruction = []
    instruction.append(random.randint(op_min, op_max)) #Instrucciones
    instruction.append(random.randint(var_min, var_max))  #Registros destinos - Solo los variables
    instruction.append(random.randint(var_min, var_max)) #Solo puede ser variable.
    
    #operador 2 es constante con probabilidad p_const
    if random_flip_coin(p_reg_op2_const):
        instruction.append(random.randint(cons_al_min, cons_in_max))
    else:
        instruction.append(random.randint(var_min, var_max))
    
    return instruction


def get_random_register(op, reg_eff=None, instruction=None):
    register = None
    if op == 1: #destino
        if not reg_eff:
            if instruction[0] < 5:
                op = 2 if random_flip_coin(0.5) else 3 #se hace que se mute uno de los registros operandos
            else: #operación unaria, se muta el operando 2 para que sea efectiva
                op = 3
        else:
            print "destino..."
            register = random.choice(reg_eff)
    
    if op == 2: #operando 1
        print "operando 1"
        register = random.randint(var_min, var_max)
    
    if op == 3: #operando 2
        print "operando 2"
        if random_flip_coin(p_reg_op2_const):
            register = random.randint(cons_al_min, cons_in_max)
        else:
            register = random.randint(var_min, var_max)
        
    return register, op



class Individual():
    """
        heigth: cantidad de instrucciones para un programa
        width: componentes de una instruccion [instrucción, destino, operador1, operador2]
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
        """
        reg_eff = set([0])
        eff_i = []
        for i in reversed(self.genomeList):
            if (i[1] in reg_eff):
                reg_eff.remove(i[1])
                
                if (i[0] < 5): #los operadores unarios tiene identificador del 5 al 9
                    reg_eff.add(i[2])
                
                if (i[3] <= var_max): #los registros constantes no pueden ser registros efectivos
                    reg_eff.add(i[3])
                
                eff_i.append(i)
    
        eff_i.reverse()     
        return eff_i
    
    
    def get_effective_instructions_with_indices(self):
        reg_eff = set([0])
        eff_i = []
        indices = []
        current_pos = len(self.genomeList)
        for i in reversed(self.genomeList):
            current_pos -= 1
            if (i[1] in reg_eff):
                reg_eff.remove(i[1])
                
                if (i[0] < 5): #los operadores unarios tiene identificador del 5 al 9
                    reg_eff.add(i[2])
                
                if (i[3] <= var_max): #los registros constantes no pueden ser registros efectivos
                    reg_eff.add(i[3])
                    
                eff_i.append(i)
                indices.append(current_pos)
            
        eff_i.reverse()
        indices.reverse()    
        return eff_i, indices
    
    
    def get_effective_constant_indices(self):
        reg_eff = set([0])
        indices = []
        current_pos = len(self.genomeList) 
        for i in reversed(self.genomeList):
            current_pos -= 1
            if (i[1] in reg_eff):
                reg_eff.remove(i[1])
                
                if (i[0] < 6): #los operadores unarios tiene identificador del 5 al 9
                    reg_eff.add(i[2]) 
                
                if (i[3] <= var_max): #los registros constantes no pueden ser registros efectivos
                    reg_eff.add(i[3])
                    
                elif(i[3] <= cons_al_max):
                    indices.append(current_pos)

        return indices
    
    
    def get_effective_registers(self, position):
        """
        intructions[i][0] = identificador de instucción
        intructions[i][1] = registro destino
        intructions[i][2] = registro operando 1
        intructions[i][3] = registro operando 2
        """
        reg_eff = set([0])
        current_pos = len(self.genomeList) 
        for i in reversed(self.genomeList):
            current_pos -= 1
            if (i[1] in reg_eff):
                reg_eff.remove(i[1])
                
                if (i[0] < 5):# los operadores unarios tiene identificador del 5 al 9
                    reg_eff.add(i[2])
                
                if (i[3] <= var_max): #los registros constantes no pueden ser registros efectivos
                    reg_eff.add(i[3])
                
            if current_pos == position or not reg_eff:
                return list(reg_eff), current_pos
        
    
    def get_program_in_python(self):
        eff_instructions = self.get_effective_instructions()
        
        program = ""
        for i in eff_instructions:
            if (i[3] >= register_offset):
                program += (operations[i[0]<<4].format(i[1], i[2], i[3] - register_offset) + '\n')
            else:
                program += (operations[i[0]].format(i[1], i[2], i[3]) + '\n')
                
        return program
    
    
    def eval_fitness(self):
        """
        Función de evaluación de fitnes
        """     
        program = self.get_program_in_python()
        #in_t tiene las mediciones en el instante t
        error_a_quad = 0
        
        try:
            for t in range(0, training_lines -1):
                in_t = r_const[t]
                r_all = copy.copy(self.r_all)
                '''
                print "program"
                print program
                print "in_t"
                print in_t
                print "t"
                print t
                print "r_all"
                
                for i in range (0, len(r_all)):
                    if (i< len(in_t)):
                        print i, r_all[i], in_t[i]
                    else:
                        print i, r_all[i]
                '''
                exec program
                
                error_a_quad += (r_all[0] - data_samples[t][index_to_predict]) ** 2

                
            error_prom_quad = error_a_quad / training_lines
            self.fitness = 1 / error_prom_quad
        except:
            """
            Si ocurre una excepción el fitness se iguala a cero (Overflow muy probablemente)
            """
            self.fitness = 0.0
            
        self.evaluated = True
        
        
    def evaluate(self):
        if not self.evaluated:
            self.eval_fitness()
        return self.fitness
    
    
    def __repr__(self):
        """ Return a string representation of Genome """
        ret = "- Individual\n"
        ret += "\tList size:\t %s\n" % (self.height)
        ret += "\tList:\t\t\t\t\t\tRegisters:\n"
        
        top = self.height if self.height > len(self.r_all) else len(self.r_all)
        for line in range(top):
            ret += "\t"
            if line < self.height:
                for item in self.genomeList[line]:
                    ret += "[%s] " % (item)
            else:
                ret += "\t\t"
            
            if line < len(self.r_all):   
                ret += "\t\t\t\t"
                ret += "r_all[%s] = " % (line)
                ret += str(self.r_all[line])
            
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