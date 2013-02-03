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
import Parameters

from Util import random_flip_coin


def ini_individual(obj):
    obj.height = random.randint(Parameters.num_min_instructions, Parameters.num_ini_instructions)
    obj.genomeList = []
    obj.fitness = 0.0      
    
    for i in range(obj.height):
        instruction = create_new_instruction()
        obj.genomeList.append(instruction)
            
    """Asegurar que la última instrucción tenga como registro destino al registro de salida"""
    obj.genomeList[len(obj.genomeList)-1][1] = Parameters.reg_out
    #obj.init_registers()
    
    obj.r_all = []
    obj.r_all += Parameters.r_out
    obj.r_all += Parameters.r_var
    
    [obj.r_all.append(random.uniform(0, Parameters.const_max)) for i in range(Parameters.cons_al_min, Parameters.cons_al_max + 1)]
    return obj


def eval_individual(individual):
    """
    Función de evaluación, dado un objeto con el atributo config_position 
    retorna una lista del error cuadrático
    para cada línea del los casos de evaluación
    """
    
    program = individual.get_program_in_python()
    #in_t tiene las mediciones en el instante t
    error_a_quad = []
#        print Parameters.r_const
    #try:
    for t in range(Parameters.training_lines, Parameters.lines):
        in_t = Parameters.r_const[t]
        r_all = copy.copy(individual.r_all)
        
        exec program
        
        error_a_quad.append((r_all[0] - Parameters.data_samples[t][individual.config_position]) ** 2)
        
        
    return error_a_quad


def create_new_instruction():
    """
    reg_sal                      -> registro de salida
    op_min, op_max               -> instrucciones
    cons_in_min, cons_in_max     -> registros de entrada constantes --> estan en r_const
    cons_al_min, cons_al_max     -> Constantes aleatorias inicializadas entre (0, random_max)
    var_min, var_max             -> registros variables inicializados a 1.
    """
    instruction = []
    instruction.append(random.randint(Parameters.op_min, Parameters.op_max)) #Instrucciones
    instruction.append(random.randint(Parameters.var_min, Parameters.var_max))  #Registros destinos - Solo los variables
    instruction.append(random.randint(Parameters.var_min, Parameters.var_max)) #Solo puede ser variable.
    
    #operador 2 es constante con probabilidad p_const
    if random_flip_coin(Parameters.p_reg_op2_const):
        #Si va a ser constante, las constantes de entrada tienen mayor probabilidad que las aleatorias.
        if random_flip_coin(Parameters.p_const_in):
            instruction.append(random.randint(Parameters.cons_in_min, Parameters.cons_in_max))
        else:
            instruction.append(random.randint(Parameters.cons_al_min, Parameters.cons_al_max))
    else:
        instruction.append(random.randint(Parameters.var_min, Parameters.var_max))
    
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
            #print "destino..."
            register = random.choice(reg_eff)
    
    if op == 2: #operando 1
        #print "operando 1"
        register = random.randint(Parameters.var_min, Parameters.var_max)
    
    if op == 3: #operando 2
        #print "operando 2"
        if random_flip_coin(Parameters.p_reg_op2_const):
            register = random.randint(Parameters.cons_al_min, Parameters.cons_in_max)
        else:
            register = random.randint(Parameters.var_min, Parameters.var_max)
        
    return register, op


def compare(x, y):
       '''
       Método para comparar dos individuos,
       si tienen el mismo fitness, se considera mejor el que tenga menos
       instrucciones efectivas
       '''
       if x.evaluate() > y.evaluate():
           return 1
       elif x.evaluate() < y.evaluate():
           return -1
       elif x.evaluate() == y.evaluate():
           if x.n_eff < y.n_eff:
               return 1
           elif x.n_eff > y.n_eff:
               return -1
           else:
               return 0

def compare_error(x, y):
       '''
       Método para comparar dos individuos por los errores obtenidos en validación,
       '''
       if x.validation_error > y.validation_error:
           return 1
       elif x.validation_error < y.validation_error:
           return -1
       elif x.validation_error == y.validation_error:
           return 0



class Individual():
    """
        heigth: cantidad de instrucciones para un programa
        width: componentes de una instruccion [instrucción, destino, operador1, operador2]
    """
    def __init__(self, width, index, config_position):
        self.width = width
        self.genomeList = []
        self.fitness= 0.0
        self.index = index
        self.evaluated = False
        self.config_position = config_position
        self.n_eff = 1
            
            
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
                
                if (i[3] <= Parameters.var_max): #los registros constantes no pueden ser registros efectivos
                    reg_eff.add(i[3])
                
                eff_i.append(i)
                
        '''
        Se setea un atributo con el número de instrucciones efectivas del individuo
        '''
        self.n_eff = len(eff_i)
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
                
                if (i[3] <= Parameters.var_max): #los registros constantes no pueden ser registros efectivos
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
                
                if (i[3] <= Parameters.var_max): #los registros constantes no pueden ser registros efectivos
                    reg_eff.add(i[3])
                    
                elif(i[3] <= Parameters.cons_al_max):
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
                
                if (i[3] <= Parameters.var_max): #los registros constantes no pueden ser registros efectivos
                    reg_eff.add(i[3])
                
            if current_pos == position or not reg_eff:
                return list(reg_eff), current_pos
        
    
    def get_program_in_python(self):
        eff_instructions = self.get_effective_instructions()
        
        program = ""
        for i in eff_instructions:
            if (i[3] >= Parameters.register_offset):
                program += (Parameters.operations[i[0]<<4].format(i[1], i[2], i[3] - Parameters.register_offset) + '\n')
            else:
                program += (Parameters.operations[i[0]].format(i[1], i[2], i[3]) + '\n')
                
        return program
    
    
    def eval_fitness(self):
        """
        Función de evaluación de fitnes
        """     
        program = self.get_program_in_python()
        #in_t tiene las mediciones en el instante t
        error_a_quad = 0
        #error_prom_quad = 0.0;
#        print Parameters.r_const
        try:
            for t in range(0, Parameters.training_lines):
                in_t = Parameters.r_const[t]
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
                
                error_a_quad += (r_all[0] - Parameters.data_samples[t][self.config_position]) ** 2
    
                    
                error_prom_quad = error_a_quad / Parameters.training_lines
            
            #para evitar la división por cero
            if error_prom_quad == 0.0:
                error_prom_quad = 0.000000001
                
            self.fitness = 1 / error_prom_quad
        
        except Exception as e:
            """
            Si ocurre una excepción el fitness se iguala a cero (Overflow muy probablemente)
            """
            print "EXCEPCION"
            print e
            print program
            print self
            print "error_prom_quad " + str(error_prom_quad)
            print "error_a_quad " + str(error_a_quad)
            self.fitness = 0.0
        
        self.evaluated = True
        
        
    def evaluate(self):
        if not self.evaluated:
            self.eval_fitness()
        return self.fitness
    
    
    def eval_validation(self):
        """
        Función de evaluación, dado un objeto con el atributo config_position 
        retorna una lista del error cuadrático
        para cada línea del los casos de evaluación
        """
        
        program = self.get_program_in_python()
        #in_t tiene las mediciones en el instante t
        #error_a_quad = []
        error_a_quad = 0.0
    #        print Parameters.r_const
        try:
            for t in range(Parameters.training_lines, Parameters.lines):
                in_t = Parameters.r_const[t]
                r_all = copy.copy(self.r_all)
                
                exec program
                
                #error_a_quad.append((r_all[0] - Parameters.data_samples[t][self.config_position]) ** 2)
                error_a_quad += (r_all[0] - Parameters.data_samples[t][self.config_position]) ** 2
    
            error_prom_quad = error_a_quad / Parameters.validation_lines
        except:
            error_prom_quad = 999999999
            
        self.validation_error = error_prom_quad
        
        return error_prom_quad


    def __repr__(self):
        """ Return a string representation of Genome """
        ret = " %s - Individual\n" % (self.index)
        ret = " Config Position: %s\n" % (self.config_position)
        ret += "\tList size:\t %s\n" % (self.height)
        ret += "\tList:\t\t\t\t\t\tRegisters:\n"
        
        top = self.height if self.height > len(self.r_all) else len(self.r_all)
        ret += "\t["
        for line in range(top):
            ret += "\t["
            if line < (self.height - 1):
                for item in self.genomeList[line]:
                    ret += "%s, " % (item)
            elif line == (self.height - 1):
                for item in self.genomeList[line]:
                    ret += "%s, " % (item)
                ret += "]]\t\t"
            else:
                ret += "],\t\t"
            
            if line < len(self.r_all):   
                ret += "\t\t\t\t"
                ret += "r_all[%s] = " % (line)
                ret += str(self.r_all[line])
            
            ret += "\n"
        
        ret+= "\tFitness:\t\t %.6f\n\n" % (self.fitness)

        ret += "\n"
        return ret    
    
    
    def clone(self):
        newcopy = copy.deepcopy(self)
        return newcopy
    
    
    def set_altered(self):
        self.fitness = 0.0
        self.evaluated = False    



if __name__ == "__main__":
    r = Individual(0, 4, 0)
    r.initialize()

    print r.genomeList
    print r.r_all