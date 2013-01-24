# -*- coding: utf-8 -*-
"""
Módulo que define las funciones asociadas al algoritmo LGP

@authors:
- U{Nahuel Hernández<mailto:jnahuelhernandez@gmail.com>}
- U{Vanessa Cañete<mailto:vanessa.can.89@gmail.com>}

@since: 1.0
"""

from multiprocessing import cpu_count

config = "1010101010101"
n = len(config)
index_to_predict = 1

k = 0
for i in config:
    if (i == '1'):
        k += 1

#DATOS
training_lines = 144
validation_lines = 96


#REGISTROS
"""
r[0] registro de salida - inicializado a 1
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
Cada instrucción se representa como una lista de 4 enteros como sigue
[operación, destino, operador1, operador2]

la convención para asegurar que el programa tenga una salida es que la
última instrucción sea del tipo [operación, 0, operador1, operador2]
"""
num_min_instructions = k
num_max_instructions = 6*k
num_ini_instructions = 2*k
num_operators = 9

#Límetes en las instrucciones
const_max = 10
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

#ALGORITMO EVOLUTIVO
num_generations = 10000
population_size = 1000000

#PROBABILIDADES
p_reg_op2_const = 0.5

"""
Las operaciones sobre la población,
como la inicialización y la evaluación de fitness se realiza 
utilizando multiprocessing de Python
"""
#Número de procesos workers
num_processors = cpu_count()
chunk_size = population_size // (num_processors * 2)


"""
Se arman las instrucciones usando la función string.format()
Según la operación se usa:

    operations[num_operation].format(id_individual, reg_dest, reg_op1, reg_op2 - reg_offset)
    
para obtener el string con la instrucción

En operaciones con dos operandos, el primero siempre es variable
y el segundo puede ser constante 
o variable con cierta probabilidad (p_reg_op2).

Para registros constantes en el segundo operador usar operations[num_operation<<4]
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