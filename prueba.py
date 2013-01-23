# -*- coding: utf-8 -*-
"""
Módulo que define las funciones asociadas al algoritmo LGP

@authors:
- U{Nahuel Hernández<mailto:jnahuelhernandez@gmail.com>}
- U{Vanessa Cañete<mailto:vanessa.can.89@gmail.com>}

@since: 1.0
"""
from sets import Set

import math
import copy

config = "1010101010101"
n = len(config)
k = 0
index_to_predict = 1

for i in config:
    if (i == '1'):
        k += 1

#REGISTROS
"""
r[0] registro de salida
r[1] .. r[k] registros aleatorios constantes
r[k + 1] .. r[3*k] registros variables inicializados a 1
r[3*k + 1] .. r[4*k] registros de entrada constantes
"""

num_registers = 5*k + 1
num_conts_registers = 2*k
num_var_register = 2*k
num_const_in_registers = k
num_out_registers = 1
num_const_random_registers = k
register_offset = 3*k + 1

#INSTRUCCIONES
"""
Cada individuo posee una lista de instrucciones
Cada instrucción se representa como una lista de 4 enteros y 1 booleano como sigue
[instrucción, destino, operador1, operador2, efectiva]
efectiva indica si la instrucción es efectiva o no, se inicializa a "false"
la convención para asegurar que el programa tenga una salida es que la primera 
"""
num_min_instructions = k
num_max_instructions = 6*k
num_ini_instructions = 2*k
num_operators = 9


#ALGORITMO EVOLUTIVO
num_generations = 10000
population_size = 1000000

#PROBABILIDADES
p_reg_op2_const = 0.5


#DATOS
training_lines = 144
validation_lines = 96


"""
para armar las instrucciones usando la función string.format()
según la operación se usa:
operations[num_operation].format(id_individual, reg_dest, reg_op1, reg_op2 - reg_offset)
para obtener el string con la instrucción

en operaciones con dos operandos, el primero siempre es variable
y el segundo puede ser constante 
o variable con cierta probabilidad (p_reg_op2)

para registros constantes en el segundo operador usar operations[num_operation<<4]
"""
operations = {  1   : 'r_all[{0}]=r_all[{1}]+r_all[{2}]',
                16  : 'r_all[{0}]=r_all[{1}]+in_t[{2}]',
                2   : 'r_all[{0}]=r_all[{1}]-r_all[{2}]',
                32  : 'r_all[{0}]=r_all[{1}]-in_t[{2}]',
                3   : 'r_all[{0}]=r_all[{1}]*r_all[{2}]',
                48  : 'r_all[{0}]=r_all[{1}]*in_t[{2}]',
                4   : 'r_all[{0}]=r_all[{1}]/r_all[{2}]',
                64  : 'r_all[{0}]=r_all[{1}]/in_t[{2}]',
                5   : 'r_all[{0}]=r_all[{1}]**r_all[{2}]',
                80  : 'r_all[{0}]=r_all[{1}]**in_t[{2}]',
                6   : 'r_all[{0}]=math.log10(r_all[{2}])',
                96  : 'r_all[{0}]=math.log10(in_t[{2}])',
                7   : 'r_all[{0}]=math.sqrt(r_all[{2}])',
                112 : 'r_all[{0}]=math.sqrt(in_t[{2}])',
                8   : 'r_all[{0}]=math.sen(r_all[{2}])',
                128 : 'r_all[{0}]=math.sen(in_t[{2}])',
                9   : 'r_all[{0}]=math.cos(r_all[{2}])',
                144 : 'r_all[{0}]=math.cos(in_t[{2}])'
                }


#INICIALIZACION DE REGISTROS DE ENTRADA CONSTANTES
"""
Una solo lista para los registros de entrada constantes.
Todos los individuos lo usan.

r_const lista de instantes, cada instante t tiene las medidas Xi
de los transformadores en donde hay un medidor


X[t][i] es el entero leido del archivo, 
medición del transformador i en el instante t
"""
r_const = []
#Para cada t en el periodo de entrenamiento
for t in range(0, training_lines):

    
    instant = []
    for i in range(0, n):
        if (config[i] == '1'):
            instant.append(X[t][i])
        
    r_const.append(instant)
    




def init_individual(t):
    #Incialización de Individuos
    pass




#ARMADO DE INSTRUCCIONES
def build_instruction():
    instucction_head = 'import math'



def mark_effective_instructions(instructions):
    """
    intructions[i][0] = identificador de instucción
    intructions[i][1] = registro destino
    intructions[i][2] = registro operando 1
    intructions[i][3] = registro operando 2
    intructions[i][4] = efectiva o no efectiva
    """
    reg_eff = set([0])
    eff_i = []
    for i in reversed(instructions):
        if (i[1] in reg_eff):
            # los operadores unarios tiene identificador del 6 al 9
            reg_eff.remove(i[1])
            
            if (i[0] < 6):
                reg_eff.add(i[2])
            
            reg_eff.add(i[3])
            #i[4] = True
            eff_i.append(i)
            
    return eff_i.reverse()


"""
se debe llamar com eval_fitness(individual.genomeList)
"""
def eval_fitness(individual):
    eff_instructions = mark_effective_instructions(individual.genomeList)
    
    program = ""
    for i in eff_instructions:
        if (i[3] >= register_offset):
            program += operations[i[0]<<4].format(i[1], i[2], i[3] - register_offset)
        else:
            program += operations[i[0]].format(i[1], i[2], i[3])
    
    #in_t tiene las mediciones en el isntante t
    error_a_quad = 0
    for t in range(0, training_lines):
        in_t = r_const[t]
        r_all = copy.copy(individual.r_all)
        exec program
        error_a_quad += (r_all[0] - X[t][index_to_predict]) ** 2
        
    fitness = 1/error_a_quad


