# -*- coding: utf-8 -*-
"""
M�dulo que define las funciones asociadas al algoritmo LGP

@authors:
- U{Nahuel Hern�ndez<mailto:jnahuelhernandez@gmail.com>}
- U{Vanessa Ca�ete<mailto:vanessa.can.89@gmail.com>}

@since: 1.0
"""

from multiprocessing import cpu_count

import Util


#INICIALIZACION DE REGISTROS DE ENTRADA CONSTANTES
def init_reg_in_const():
    """
    Una solo lista para los registros de entrada constantes. Todos los individuos lo usan.
    
    r_const lista de instantes, cada instante t tiene las medidas Xi de los transformadores en donde hay un medidor
    
    X[t][i] es el entero leido del archivo, medici�n del transformador i en el instante t
    """
    #Para cada t en el periodo de entrenamiento
    const = []

    for t in range(0, lines):
        instant = []
        for i in range(0, n):
            if (config[i] == '1'):
                instant.append(data_samples[t][i])
            
        const.append(instant)
        
    return const

def read_samples():
    n_data, lines_data, data = Util.get_matrix_from_file(filename)
    
    if ( n_data != n or lines_data != lines):
        print "El archivo no coincide con los par�metros"
        exit(-1)
        
    return data
    
config = "1010101010" + "1010101010" + "1010101010" + "1010101010"
n = len(config)
index_to_predict = 1

k = config.count('1')

#DATOS
filename = "Datos60.txt"
lines = 248
training_lines = 144
validation_lines = 104
#r_const = None
#data_samples = []
#Constante m�xima para inicializaci�n de registros
const_max = 10.0
step_size_const = 2


#INSTRUCCIONES
"""
Cada individuo posee una lista de instrucciones
Cada instrucci�n se representa como una lista de 4 enteros como sigue
[operaci�n, destino, operador1, operador2]

la convenci�n para asegurar que el programa tenga una salida es que la
�ltima instrucci�n sea del tipo [operaci�n, 0, operador1, operador2]
"""
num_min_instructions = k
num_max_instructions = 8*k
num_ini_instructions = 2*k
num_operators = 9

num_registers = 5*k + 1
num_conts_registers = 2*k
num_var_register = k
num_const_in_registers = k
num_out_registers = 1
num_const_random_registers = k

#L�mites en las instrucciones
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

#ALGORITMO EVOLUTIVO
num_generations = 10000
population_size = 500

freq_stats=100
pool_size = 4


#PROBABILIDADES
p_reg_op2_const = 0.7 #probabilidad cambiada 0.5 original 
p_ins = 0.7 #prob. de insertar mayor a la de borrar
p_del = 0.3

p_regmut = 0.5 #Mutacion de registros
p_opermut = 0.40 #Mutacion de operadores
p_constmut = 0.1 #Mutacion de constantes
p_const_in = 0.7 #probabilidad de que si un operando es constante sea de entrada

p_micro_mutation = 0.95
p_crossover = 0.05 #original 0.15
p_macro_mutation = 0.45

"""
Las operaciones sobre la poblaci�n,
como la inicializaci�n y la evaluaci�n de fitness se realiza 
utilizando multiprocessing de Python
"""
#N�mero de procesos workers
num_processors = cpu_count()
chunk_size = population_size // (num_processors * 2)


"""
Se arman las instrucciones usando la funci�n string.format()
Seg�n la operaci�n se usa:

    operations[num_operation].format(id_individual, reg_dest, reg_op1, reg_op2 - reg_offset)
    
para obtener el string con la instrucci�n

En operaciones con dos operandos, el primero siempre es variable
y el segundo puede ser constante 
o variable con cierta probabilidad (p_reg_op2).

Para registros constantes en el segundo operador usar operations[num_operation<<4]

Para operaciones ilegales como:
    Divisi�n por 0
    logaritmo de un n�mero negativo
    N�mero negativo elevado a un fraccionario (se hac�a)
5   : 'r_all[{0}]=(r_all[{1}]**r_all[{2}]) if not (r_all[{1}] < 0 and not (r_all[{2}]).is_integer()) or (r_all[{2}] > 100) else 1.0',
80  : 'r_all[{0}]=r_all[{1}]**in_t[{2}] if not (r_all[{1}] < 0 and not (in_t[{2}]).is_integer()) or (in_t[{2}] > 100) else 1.0',

    Ra�z cuadrada de un n�mero negativo
se hace que el resultado de la operaci�n sea 1.0

El operador de potencia pas� a ser un operador de elevado al cuadrado

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

data_samples = read_samples()
r_const = init_reg_in_const()
