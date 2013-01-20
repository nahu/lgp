

config = "1010101010101"
n = len(config)
k = 0

for i in config:
    if (i == '1'):
        k++

#REGISTROS
"""
r[0] registro de salida
r[1] .. r[k] registros de entrada constantes
r[k+1] .. r[2*k] registros aleatorios constantes
r[2*k + 1] .. r[4*k] registros variables inicializados a 1
"""
num_registers = 5*k + 1
num_conts_registers = 2*k
num_var_register = 2*k
num_const_in_registers = k
num_out_registers = 1
num_const_random_registers = k

#INSTRUCCIONES
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



#para armar las instrucciones usando la función string.format()
#según la operación se usa:
#operators[num_operation].format(id_individual, reg_dest, reg_op1 - num_conts_registers, reg_op2)
#para obtener el string con la instrucción

#en operaciones con dos operandos, el primero siempre es variable
#y el segundo puede ser constante 
#o variable con cierta probabilidad (p_reg_op2)

#para registros constantes en el segundo operador usar operators[num_operation<<4]
operators = {   1   : 'r_all[{0}][{1}]=r_all[{0}][{2}]+r_all[{0}][{3}]',
                16  : 'r_all[{0}][{1}]=r_all[{0}][{2}]+r_const[{3}]',
                2   : 'r_all[{0}][{1}]=r_all[{0}][{2}]-r_all[{0}][{3}]',
                32  : 'r_all[{0}][{1}]=r_all[{0}][{2}]-r_const[{3}]',
                3   : 'r_all[{0}][{1}]=r_all[{0}][{2}]*r_all[{0}][{3}]',
                48  : 'r_all[{0}][{1}]=r_all[{0}][{2}]*r_const[{3}]',
                4   : 'r_all[{0}][{1}]=r_all[{0}][{2}]/r_all[{0}][{3}]',
                64  : 'r_all[{0}][{1}]=r_all[{0}][{2}]/r_const[{3}]',
                5   : 'r_all[{0}][{1}]=r_all[{0}][{2}]**r_all[{0}][{3}]',
                80  : 'r_all[{0}][{1}]=r_all[{0}][{2}]**r_const[{3}]',
                6   : 'r_all[{0}][{1}]=math.log10(r_all[{0}][{3}])',
                96  : 'r_all[{0}][{1}]=math.log10(r_const[{3}])',
                7   : 'r_all[{0}][{1}]=math.sqrt(r_all[{0}][{3}])',
                112 : 'r_all[{0}][{1}]=math.sqrt(r_const[{3}])',
                8   : 'r_all[{0}][{1}]=math.sen(r_all[{0}][{3}])',
                128 : 'r_all[{0}][{1}]=math.sen(r_const[{3}])',
                9   : 'r_all[{0}][{1}]=math.cos(r_all[{0}][{3}])',
                144 : 'r_all[{0}][{1}]=math.cos(r_const[{3}])'
                }

def init_individual(t):
    #Incialización de Individuos

    for i in range()

#ARMADO DE INSTRUCCIONES
def build_instruction():
    instucction_head = 'import math'


def eval_fitness():

    #Para cada t en el periodo de entrenamiento
    for t in range(0, training_lines):
        #Un solo vector para los registros de entrada constantes,
        #todos los individuos lo usan

        #X[t][i] es el entero leido del archivo, 
        #medición del transformador i en el instante t

        index = 1
        for i in range(0, n):
            if (config[i] == '1'):
                r_const[index++] = X[t][i]

        r_all se toca en la mutacion
