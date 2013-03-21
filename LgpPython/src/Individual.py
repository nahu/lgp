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

import Util 

def ini_individual(obj):
    obj.height = random.randint(Parameters.num_min_instructions, Parameters.num_ini_instructions)
    obj.genomeList = []
    obj.fitness = 0.0      
    
    for i in range(obj.height):
        instruction = create_new_instruction()
        obj.genomeList.append(instruction)
            
    """
    Asegurar que la última instrucción tenga como registro destino al registro de salida
    """
    obj.genomeList[len(obj.genomeList) - 1][1] = Parameters.reg_out
    
    obj.r_all = []
    obj.r_all += Parameters.r_out
    obj.r_all += Parameters.r_var
    
    [obj.r_all.append(random.uniform(Parameters.const_min, Parameters.const_max)) for i in range(Parameters.cons_al_min, Parameters.cons_al_max + 1 -4)]
    obj.r_all.append(1); obj.r_all.append(0); obj.r_all.append(math.pi); obj.r_all.append(math.e); 
    
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
    '''Cambio: se permite al primer operando ser variable o constante de entrada'''
    if Util.random_flip_coin(0.5):
        instruction.append(random.randint(Parameters.var_min, Parameters.var_max)) #Solo puede ser variable.
    else:
        instruction.append(random.randint(Parameters.cons_in_min, Parameters.cons_in_max))
    
    #operador 2 es constante con probabilidad p_const
    if Util.random_flip_coin(Parameters.p_reg_op2_const):
        #Si va a ser constante, las constantes de entrada tienen mayor probabilidad que las aleatorias.
        if Util.random_flip_coin(Parameters.p_const_in):
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
                op = 2 if Util.random_flip_coin(0.5) else 3 #se hace que se mute uno de los registros operandos
            else: #operación unaria, se muta el operando 2 para que sea efectiva
                op = 3
        else:
            #print "destino..."
            register = random.choice(reg_eff)
    
    if op == 2: #operando 1
        '''Cambio: se permite al primer operando ser variable o constante de entrada'''
        if Util.random_flip_coin(0.5):
            register = random.randint(Parameters.var_min, Parameters.var_max)
        else:
            register = random.randint(Parameters.cons_in_min, Parameters.cons_in_max)
        register = random.randint(Parameters.var_min, Parameters.var_max)
    
    if op == 3: #operando 2
        #print "operando 2"
        #operador 2 es constante con probabilidad p_const
        if Util.random_flip_coin(Parameters.p_reg_op2_const):
            #Si va a ser constante, las constantes de entrada tienen mayor probabilidad que las aleatorias.
            if Util.random_flip_coin(Parameters.p_const_in):
                register = random.randint(Parameters.cons_in_min, Parameters.cons_in_max)
            else:
                register = random.randint(Parameters.cons_al_min, Parameters.cons_al_max)
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

def compare_error_prom(x, y):
    '''
    Método para comparar dos individuos,
    por  el error promedio en la evaluación del fitness
    '''
    if x.evaluate(obj=1) > y.evaluate(obj=1):
        return -1
    elif x.evaluate(obj=1) < y.evaluate(obj=1):
        return 1
    elif x.evaluate(obj=1) == y.evaluate(obj=1):
        if x.n_eff < y.n_eff:
            return 1
        elif x.n_eff > y.n_eff:
            return -1
        else:
            return 0

def compare_deviation_in_error(x, y):
    '''
    Método para comparar dos individuos,
    compara la desviación típica de los errores obtenidos
    en la evaluación del fitness
    '''
    if x.evaluate(obj=2) > y.evaluate(obj=2):
        return -1
    elif x.evaluate(obj=2) < y.evaluate(obj=2):
        return 1
    elif x.evaluate(obj=2) == y.evaluate(obj=2):
        if x.n_eff < y.n_eff:
            return 1
        elif x.n_eff > y.n_eff:
            return -1
        else:
            return 0
        
def compare_validation_error(x, y):
    '''
    Método para comparar dos individuos por los errores obtenidos en validación,
    '''
    if x.validation_error > y.validation_error:
        return 1
    elif x.validation_error < y.validation_error:
        return -1
    elif x.validation_error == y.validation_error:
        return 0

def check_destination_register(obj):
    for i in obj.genomeList:
        if i[1] != 0 and (i[1]< Parameters.var_min or i[1] > Parameters.var_max):
            print "EL REGISTRO DESTINO ES INCORRECTO -- " 
            print i  

class Individual():
    """
        heigth: cantidad de instrucciones para un programa
        width: componentes de una instruccion [instrucción, destino, operador1, operador2]
    """
    def __init__(self, width, index, config_position):
        self.width = width
        self.genomeList = []
        self.fitness = 0.0
        self.error = -1
        self.dev = -1
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
                    if (i[2] <= Parameters.var_max):
                        reg_eff.add(i[2])
                
                if (i[3] <= Parameters.var_max): #los registros constantes no pueden ser registros efectivos
                    reg_eff.add(i[3])
                
            if current_pos == position or not reg_eff:
                return list(reg_eff), current_pos
        
    
    def get_program_in_python(self):
        eff_instructions = self.get_effective_instructions()
        program = ""
        for i in eff_instructions:
            '''
            Como los registros de entrada se encuentra en una lista separada al conjunto de registros r_all
            Se realiza un corrimiento de Parameters.register_offset (tamaño de r_all) 
            al registro de la instrucción para que entre en el rango de r_const
            variable  variable  - sin corrimiento
            entrada  entrada    - << 4
            variable entrada    - << 8
            entrada variable    - <<11
            '''
            i0 = i[0]
            i2 = i[2] - Parameters.register_offset if i[2] >= Parameters.register_offset else i[2]
            i3 = i[3] - Parameters.register_offset if i[3] >= Parameters.register_offset else i[3]
            ''' Para operaciones de dos operandos'''
            if (i[0]<5):
                if (i[2] >= Parameters.register_offset): #i2 es de entrada
                    if (i[3] >= Parameters.register_offset): #i3 es de entrada
                        i0 = i[0]<<4 #entrada  entrada   - << 4
                    else:
                        i0 = i[0]<<11 #entrada variable  - << 11
                else:
                    if i[3] >= Parameters.register_offset:
                        i0 = i[0]<<8 #variable entrada    - << 8
                
            else:
                if (i[3] >= Parameters.register_offset):
                    i0 = i[0]<<4 #entrada  entrada   - << 4
            program += (Parameters.operations[i0].format(i[1], i2, i3) + '\n')
        return program
    
    
    def eval_fitness(self):
        """
        Función de evaluación de fitnes
        """     
        program = self.get_program_in_python()
        list_errors = []
        error_a_quad = 0.0
        error_prom_quad = 0.0
        
        try:
            for t in range(0, Parameters.training_lines):
                #in_t tiene las mediciones en el instante t
                in_t = Parameters.r_const[t]
                r_all = copy.copy(self.r_all)
                exec program
                error_t = (r_all[0] - Parameters.data_samples[t][self.config_position]) ** 2
                error_a_quad += error_t
                list_errors.append(error_t)
                
            error_prom_quad = error_a_quad / Parameters.training_lines
            
            '''Se halla la desviacion tipica del error. (Sumatoria(error_medio - errorXLinea)/training_lines'''
            error_desv = 0.0
            
            for error_t in list_errors:
                error_desv += (error_prom_quad - error_t) ** 2
            error_desv /= (Parameters.training_lines - 1)
            
            #para evitar la división por cero
            if error_prom_quad == 0.0:
                error_prom_quad = 0.000000001
                
            self.error = error_prom_quad
            self.dev = math.sqrt(error_desv)
                
            self.fitness = 1 / ((Parameters.w_ob1 * error_prom_quad) + (Parameters.w_ob2 * self.dev))
        
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
        
        
    def evaluate(self, obj=0):
        if not self.evaluated:
            self.eval_fitness()
        
        if (obj == 0):
            return self.fitness
        elif (obj == 1):
            return self.error
        elif (obj == 2):
            return self.dev
        
        print "ERROR, OBJETIVO NO ENCONTRADO"
    
    
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
        ret = "#Index: %s\n" % (self.index)
        ret += "#Config Position: %s\n" % (self.config_position)
        ret += "#Training Error: %s\n" % (self.error)
        ret += "#Training Deviation: %s\n" % (self.dev)
        ret += "#List size:\t %s\n" % (self.height)
#        ret += "#List:\n"
        

#        ret += "genome_list = [ \n"
#        for line in range(self.height):
#            ret += ""
#            if line < (self.height - 1):
#                item = str(self.genomeList[line])
#                ret += "%s,\n" % (item)
#            elif line == (self.height - 1):
#                ret += "%s]\n\n" % (item)
#                
#        ret += "#Registers:\n"
#        for line in range(len(self.r_all)):
#            ret += "r_all[%s] = " % (line)
#            ret += str(self.r_all[line])
#            ret += "\n"
#        ret += "\n"
        ret += "# Constant Registers:\n"
        offset = Parameters.num_var_register
        for line in range(Parameters.num_const_random_registers):
            
            ret += "r_all[%s] = " % (offset + line + 1)
            ret += str(self.r_all[offset + line + 1])
            ret += "  -  "
        ret += "\n"
        ret+= "#Fitness:\t %.15f\n\n" % (self.fitness)

        return ret    
    
    
    def clone(self):
        newcopy = copy.deepcopy(self)
        return newcopy
    
    
    def set_altered(self):
        self.fitness = 0.0
        self.evaluated = False
    
    def set_index (self, index):
        self.index = index
        return self
    
    
def exec_program(program, all_r):
    config_position = 1
    w_ob1 = 1
    w_ob2 = 1
    
    list_errors = []
    list_estimations = [config_position]
    error_a_quad = 0.0
    error_prom_quad = 0.0
    
    for t in range(0, Parameters.training_lines):
        #in_t tiene las mediciones en el instante t
        in_t = Parameters.r_const[t]
        r_all = copy.copy(all_r)
        exec program
        error_t = (r_all[0] - Parameters.data_samples[t][config_position]) ** 2
        error_a_quad += error_t
        list_errors.append(error_t)
        list_estimations.append(r_all[0])
        
    error_prom_quad = error_a_quad / Parameters.training_lines
    
    '''Se halla la desviacion tipica del error. (Sumatoria(error_medio - errorXLinea)/training_lines'''
    error_desv = 0.0
    
    print "---------------------"
    print "Errores en Entrenamiento"
    print "---------------------"
    for error_t in list_errors:
        print error_t
        error_desv += (error_prom_quad - error_t) ** 2
    error_desv /= (Parameters.training_lines - 1)
    
    #para evitar la división por cero
    if error_prom_quad == 0.0:
        error_prom_quad = 0.000000001
        
    error = error_prom_quad
    dev = math.sqrt(error_desv)
        
    fitness = 1 / ((w_ob1 * error_prom_quad) + (w_ob2 * dev))
    
    print "error promedio training\t\t" + str(error)
    print "deviation\t\t\t" + str(dev) 
    print "fitness\t\t\t\t" + str(fitness)

    #In evalutation
    error_a_quad = []

    for t in range(Parameters.training_lines, Parameters.lines):
        in_t = Parameters.r_const[t]
        r_all = copy.copy(all_r)
        
        exec program
        
        error_a_quad.append((r_all[0] - Parameters.data_samples[t][config_position]) ** 2)
        list_estimations.append(r_all[0])
        
    with_prom = Util.sum_errors(error_a_quad)
    print "---------------------"
    print "Errores en validación"
    print "---------------------"
    for i in with_prom[:-2]:
        print i
    print "error total validación\t\t" + str(with_prom[-2])
    print "error promedio validación\t" + str(with_prom[-1])
    
    f_estimations = "estimaciones_transf-" + str(config_position) + ".csv"
    Util.estimations_to_file(f_estimations, list_estimations)
    
if __name__ == "__main__":
#    r = Individual(4, 0, 1)
#    r = ini_individual(r)
#    r.evaluate()
    program = \
"""
r_all[2] = math.sin(in_t[9])
r_all[1] = math.log10(abs(in_t[17])) if in_t[17] != 0 else in_t[17] + Parameters.c_undef
r_all[3] = r_all[1] * r_all[20]
r_all[9] = math.sin(in_t[34])
r_all[4] = math.sin(r_all[2])
r_all[5] = r_all[3] + in_t[12]
r_all[7] = r_all[9] + in_t[24]
r_all[6] = math.sin(r_all[5])
r_all[2] = r_all[4] * r_all[7]
r_all[4] = math.sin(r_all[6])
r_all[3] = math.sqrt(abs(in_t[23]))
r_all[2] = r_all[2] + in_t[25]
r_all[7] = r_all[4] * r_all[2]
r_all[3] = r_all[3] - r_all[7]
r_all[8] = r_all[3] + in_t[30]
r_all[2] = math.sqrt(abs(r_all[8]))
r_all[3] = math.sqrt(abs(r_all[2]))
r_all[2] = (r_all[3] / r_all[17]) if r_all[17] != 0 else r_all[3] + Parameters.c_undef
r_all[3] = math.cos(r_all[2])
r_all[5] = math.cos(r_all[3])
r_all[0] = r_all[5] * in_t[1]
"""

    r_all = []
    for i in range(21):
        r_all.append(1.0)
    
    r_all[11] = 1.7094056767
    r_all[12] = 2.41783652885
    r_all[13] = 19.4438579255
    r_all[14] = 4.11038813082
    r_all[15] = 11.8269584515
    r_all[16] = 2.34877830156
    r_all[17] = 5.38748139937
    r_all[18] = 0.939769686092
    r_all[19] = 13.1294431722
    r_all[20] = 5.2#20.7
#    
#    r_all_best = copy.copy(r_all)
#    
#    
#    
#    
    program = \
"""
r_all[2] = r_all[4] * in_t[26]
r_all[5] = math.sqrt(abs(r_all[2]))
r_all[3] = r_all[5] * in_t[13]
r_all[8] = math.cos(r_all[3])
r_all[5] = math.cos(r_all[2])
r_all[7] = math.log10(abs(r_all[8])) if r_all[8] != 0 else r_all[8] + Parameters.c_undef
r_all[1] = math.cos(r_all[7])
r_all[4] = r_all[1] - r_all[5]
r_all[4] = math.sin(r_all[4])
r_all[1] = math.log10(abs(r_all[4])) if r_all[4] != 0 else r_all[4] + Parameters.c_undef
r_all[9] = (r_all[1] / r_all[4]) if r_all[4] != 0 else r_all[1] + Parameters.c_undef
r_all[4] = r_all[9] ** 2
r_all[6] = r_all[4] - r_all[13]
r_all[9] = r_all[4] - in_t[20]
r_all[3] = r_all[6] + r_all[9]
r_all[3] = r_all[3] + in_t[0]
r_all[1] = r_all[3] - r_all[13]
r_all[3] = math.sqrt(abs(r_all[1]))
r_all[6] = math.log10(abs(in_t[23])) if in_t[23] != 0 else in_t[23] + Parameters.c_undef
r_all[1] = math.sin(in_t[25])
r_all[9] = r_all[6] * r_all[1]
r_all[7] = math.sqrt(abs(r_all[12]))
r_all[8] = r_all[9] - in_t[4]
r_all[6] = r_all[7] * r_all[8]
r_all[3] = math.log10(abs(r_all[3])) if r_all[3] != 0 else r_all[3] + Parameters.c_undef
r_all[4] = r_all[3] * r_all[9]
r_all[1] = math.cos(r_all[4])
r_all[9] = math.sin(r_all[1])
r_all[5] = r_all[6] * r_all[9]
r_all[2] = r_all[5] - in_t[17]
r_all[8] = r_all[5] + in_t[17]
r_all[6] = r_all[2] * r_all[5]
r_all[4] = math.sin(in_t[33])
r_all[2] = r_all[8] * r_all[4]
r_all[2] = r_all[2] + in_t[7]
r_all[3] = math.log10(abs(r_all[6])) if r_all[6] != 0 else r_all[6] + Parameters.c_undef
r_all[7] = (r_all[3] / in_t[11]) if in_t[11] != 0 else r_all[3] + Parameters.c_undef
r_all[4] = r_all[7] * r_all[2]
r_all[0] = r_all[4] + in_t[30]
"""
    r_all = []
    for i in range(16):
        r_all.append(1.0)
    
    r_all[10] = 10.4067942272
    r_all[11] = 3.02338265751
    r_all[12] = 8.31293458192
    r_all[13] = 30.5#31.8578269733
    r_all[14] = 21.8569003403
    r_all[15] = 7.10555095774
#    
    program = \
"""
r_all[6] = in_t[2] ** 2
r_all[1] = (r_all[6] / in_t[1]) if in_t[1] != 0 else r_all[6] + Parameters.c_undef
r_all[1] = r_all[1] + in_t[25]
r_all[9] = math.sqrt(abs(r_all[1]))
r_all[2] = r_all[9] - in_t[34]
r_all[4] = r_all[2] - r_all[13]
r_all[9] = r_all[4] + in_t[1]
r_all[4] = in_t[8] ** 2
r_all[9] = r_all[9] ** 2
r_all[7] = math.sqrt(abs(r_all[4]))
r_all[6] = (r_all[7] / r_all[9]) if r_all[9] != 0 else r_all[7] + Parameters.c_undef
r_all[5] = math.cos(r_all[6])
r_all[7] = (r_all[5] / r_all[11]) if r_all[11] != 0 else r_all[5] + Parameters.c_undef
r_all[8] = math.sin(r_all[7])
r_all[8] = r_all[8] * in_t[24]
r_all[4] = math.log10(abs(r_all[8])) if r_all[8] != 0 else r_all[8] + Parameters.c_undef
r_all[5] = in_t[0] ** 2
r_all[2] = math.cos(r_all[5])
r_all[2] = (r_all[2] / r_all[4]) if r_all[4] != 0 else r_all[2] + Parameters.c_undef
r_all[6] = r_all[2] - in_t[18]
r_all[2] = math.cos(r_all[6])
r_all[5] = math.cos(in_t[22])
r_all[5] = r_all[5] ** 2
r_all[6] = in_t[5] ** 2
r_all[9] = r_all[5] * in_t[13]
r_all[4] = r_all[6] - r_all[5]
r_all[7] = r_all[9] * r_all[9]
r_all[9] = (r_all[7] / r_all[4]) if r_all[4] != 0 else r_all[7] + Parameters.c_undef
r_all[6] = r_all[9] - in_t[3]
r_all[6] = r_all[2] * r_all[6]
r_all[5] = math.cos(in_t[8])
r_all[1] = r_all[5] * in_t[27]
r_all[5] = r_all[6] - r_all[14]
r_all[2] = r_all[1] - r_all[5]
r_all[8] = math.cos(in_t[16])
r_all[1] = math.sqrt(abs(in_t[10]))
r_all[3] = (r_all[2] / r_all[1]) if r_all[1] != 0 else r_all[2] + Parameters.c_undef
r_all[7] = math.sin(r_all[8])
r_all[3] = r_all[7] * r_all[3]
r_all[4] = r_all[3] - in_t[31]
r_all[8] = r_all[4] * in_t[18]
r_all[0] = math.sqrt(abs(r_all[8]))
"""

    r_all = []
    for i in range(16):
        r_all.append(1.0)
    
    r_all[10] = 24.7008285724
    r_all[11] = 15.001317441
    r_all[12] = 19.996622157
    r_all[13] = 13.778801291
    r_all[14] = 23.7290797537
    r_all[15] = 6.392450299
    exec_program(program, r_all)
    

    
        