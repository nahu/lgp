# -*- coding: utf-8 -*-
"""
Módulo que define las funciones asociadas al algoritmo LGP

@authors:
- U{Nahuel Hernández<mailto:jnahuelhernandez@gmail.com>}
- U{Vanessa Cañete<mailto:vanessa.can.89@gmail.com>}

@since: 1.0
"""

from multiprocessing import cpu_count

import Util


#INICIALIZACION DE REGISTROS DE ENTRADA CONSTANTES
def init_reg_in_const():
    """
    Una solo lista para los registros de entrada constantes. Todos los individuos lo usan.
    r_const lista de instantes, cada instante t tiene las medidas Xi de los transformadores en donde hay un medidor
    X[t][i] es el entero leido del archivo, medición del transformador i en el instante t
    """
    #Para cada t en el periodo de entrenamiento
    const = []

    for t in range(lines):
        instant = []
        for i in range(n):
            if (config[i] == '1'):
                instant.append(data_samples[t][i])
            
        const.append(instant)
        
    return const

def read_samples():
    n_data, lines_data, data = Util.get_matrix_from_file(filename)
    
    if ( n_data != n or lines_data != lines):
        print "El archivo no coincide con los parámetros"
        exit(-1)
        
    return data

#Solo faltan 5 transformadores. [1,7,15,23,39]
config = "1011111011" + "1111101111" + "1110111111" + "1111111110"
n = len(config)
index_to_predict = 1

k = config.count('1')

#DATOS
filename = "../data/Datos60.txt"
lines = 248
training_lines = 144
validation_lines = 104



#INSTRUCCIONES
"""
Cada individuo posee una lista de instrucciones
Cada instrucción se representa como una lista de 4 enteros como sigue
[operación, destino, operador1, operador2]

la convención para asegurar que el programa tenga una salida es que la
última instrucción sea del tipo [operación, 0, operador1, operador2]
"""
num_min_instructions = k
num_max_instructions = 10*k
num_ini_instructions = 4*k
num_operators = 9

num_registers = 5*k + 1
num_conts_registers = 2*k
num_var_register = k
num_const_in_registers = k
num_out_registers = 1
num_const_random_registers = k

#Límites en las instrucciones
reg_out = 0
var_min = 1
var_max = num_var_register
cons_al_min = var_max + 1
cons_al_max = var_max + num_const_random_registers #3*k
cons_in_min = cons_al_max + 1
cons_in_max = cons_al_max + num_const_in_registers
op_min = 1 
op_max = 9
register_offset = cons_al_max + 1

#REGISTROS
"""
r[0] registro variable de salida - inicializado a 1
r[1] .. r[k] registros variables inicializados a 1
r[k + 1] .. r[2*k] registros aleatorios constantes
r[2*k + 1] .. r[3*k] registros de entrada constantes
"""
r_out = [1.0]
r_var = []
[r_var.append(1.0) for i in range(var_min, var_max + 1)]


"""
Se arman las instrucciones usando la función string.format()
Según la operación se usa:

    operations[num_operation].format(id_individual, reg_dest, reg_op1, reg_op2 - reg_offset)
    
para obtener el string con la instrucción

En operaciones con dos operandos, el primero siempre es variable
y el segundo puede ser constante 
o variable con cierta probabilidad (p_reg_op2).

Para registros constantes en el segundo operador usar operations[num_operation<<4]

Para operaciones ilegales como:
    División por 0
    logaritmo de un número negativo
    Número negativo elevado a un fraccionario (se hacía)
5   : 'r_all[{0}]=(r_all[{1}]**r_all[{2}]) if not (r_all[{1}] < 0 and not (r_all[{2}]).is_integer()) or (r_all[{2}] > 100) else 1.0',
80  : 'r_all[{0}]=r_all[{1}]**in_t[{2}] if not (r_all[{1}] < 0 and not (in_t[{2}]).is_integer()) or (in_t[{2}] > 100) else 1.0',

    Raíz cuadrada de un número negativo
se hace que el resultado de la operación sea 1.0

El operador de potencia pasó a ser un operador de elevado al cuadrado

"""
operations = {  1   : 'r_all[{0}] = r_all[{1}] + r_all[{2}]',
                16  : 'r_all[{0}] = r_all[{1}] + in_t[{2}]',
                2   : 'r_all[{0}] = r_all[{1}] - r_all[{2}]',
                32  : 'r_all[{0}] = r_all[{1}] - in_t[{2}]',
                3   : 'r_all[{0}] = r_all[{1}] * r_all[{2}]',
                48  : 'r_all[{0}] = r_all[{1}] * in_t[{2}]',
                4   : 'r_all[{0}] = (r_all[{1}] / r_all[{2}]) if r_all[{2}] != 0 else 1.0',
                64  : 'r_all[{0}] = (r_all[{1}] / in_t[{2}]) if in_t[{2}] != 0 else 1.0',
                5   : 'r_all[{0}] = r_all[{2}] ** 2',
                80  : 'r_all[{0}] = in_t[{2}] ** 2',
                6   : 'r_all[{0}] = math.log10(r_all[{2}]) if r_all[{2}] > 0 else 1.0',
                96  : 'r_all[{0}] = math.log10(in_t[{2}]) if in_t[{2}] > 0 else 1.0',
                7   : 'r_all[{0}] = math.sqrt(r_all[{2}]) if r_all[{2}] >= 0 else 1.0',
                112 : 'r_all[{0}] = math.sqrt(in_t[{2}]) if in_t[{2}] >= 0 else 1.0',
                8   : 'r_all[{0}] = math.sin(r_all[{2}])',
                128 : 'r_all[{0}] = math.sin(in_t[{2}])',
                9   : 'r_all[{0}] = math.cos(r_all[{2}])',
                144 : 'r_all[{0}] = math.cos(in_t[{2}])'
                }

'''
************************************  ALGORITMO EVOLUTIVO ***********************************

    num_generations: cantidad de generaciones de individuos en el algoritmo evolutivo
    population_size: el tamaño de la población.
    demes: cantidad de subpoblaciones que se procesarán en paralelo.
    freq_stats: cada cuantas generaciones se imprimirá el estado del algoritmo.
    pool_size: cantidad de individuos qeu participaran del torneo
    #migration_gen = 0.10 * num_generation
'''
num_generations = 2000
population_size = 8000
demes = 8
freq_stats= 100
pool_size = 5

'''
***************************************  PROBABILIDADES ***************************************

    p_reg_op2_const: probabilidad de que el segundo operando de una instrucción sea constante
    p_const_in: probabilidad de que si un operando es constante sea de entrada
        const_max: Constante máxima para inicialización de registros

    p_macro_mutation: probabilidad de que un individuo sea sometido al proceso 
                    de macro mutación (agregacion/eliminación de instrucciones)
        p_ins: probabilidad de agregar una instrucciòn. Segùn el libro mayor a la de eliminar.
        p_del: probabilidad de eliminar una instrucción.    

    p_micro_mutation: probabilidad de que un individuo sea sometido al 
                      proceso de micro mutación (mutación interna de instrucciones)
        p_regmut: probabilidad de que se aplique la micro mutación a los registros
        p_opermut: probabilidad de que se aplique la micro mutación a las operaciones
        p_constmut: probabilidad de que se aplique la micro mutación a las constantes 
                    utilizando step_size_const para la variación
        step_size_const: variacion de las constantes cuando hay una mutacion de registros
            
    p_crossover: probabilidad de carplia la operación de cruzamiento.
'''
p_reg_op2_const = 0.7
p_const_in = 0.75
const_max = 50

p_macro_mutation = 0.45
p_ins = 0.75
p_del = 0.25

p_micro_mutation = 0.90
p_regmut = 0.5
p_opermut = 0.45
p_constmut = 0.05
step_size_const = 2

p_crossover = 0.05

'''
*************************************  MULTIPROCESAMIENTO *************************************
Las operaciones sobre la población,como la inicialización y la evaluación de fitness se realiza 
utilizando multiprocessing de Python
'''
num_processors = cpu_count() #Número de procesos workers
chunk_size = (population_size // demes) // num_processors


'''
****************************************  CARGA DE DATOS ***************************************
                Carga de datos e inicialización de registros de entrada
'''
data_samples = read_samples()
r_const = init_reg_in_const()
