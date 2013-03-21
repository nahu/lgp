/*
 * parameters.h
 *
 *  Created on: Mar 16, 2013
 *      Author: Vanessa Cañete, Nahuel Hernández
 */

//poner todo en mayúsculas lo de parameters.py con defines y cambiar si algun cambio con respecto al paper

#include <string.h>

//Solo faltan 5 transformadores. [1,7,15,23,39]
std::string CONFIG = "1011111011" + "1111101111" + "1110111111" + "1111111110";
int N;
int K;

#define INDEX_TO_PREDICT 1


/******************************* DATOS **********************************/

#define FILENAME "../data/Datos60.txt"
#define LINES 248
#define TRAINING_LINES 200
#define VALIDATION_LINES 48

#define W_OB1 1
#define W_OB2 3


/*INSTRUCCIONES

Cada individuo posee una lista de instrucciones
Cada instrucción se representa como una lista de 4 enteros como sigue
[operación, destino, operador1, operador2]

la convención para asegurar que el programa tenga una salida es que la
última instrucción sea del tipo [operación, 0, operador1, operador2]
*/

#define NUM_MIN_INSTRUCTIONS 3 * K
#define NUM_MAX_INSTRUCTIONS 12 * K
#define NUM_INI_INSTRUCTIONS 6 * K
#define NUM_OPERATORS 9


#define NUM_VAR_REGISTER 9 //K
#define NUM_CONST_IN_REGISTERS K
#define NUM_OUT_REGISTERS 1
#define NUM_CONST_RANDOM_REGISTERS 6 //K
#define NUM_CONTS_REGISTERS NUM_CONST_RANDOM_REGISTERS + NUM_CONST_IN_REGISTERS
#define NUM_REGISTERS NUM_CONTS_REGISTERS + NUM_VAR_REGISTER + NUM_CONST_IN_REGISTERS + NUM_OUT_REGISTERS + NUM_CONST_RANDOM_REGISTERS


//Límites en las instrucciones
#define REG_OUT 0
#define VAR_MIN 1
#define VAR_MAX NUM_VAR_REGISTER
#define CONS_AL_MIN VAR_MAX + 1
#define CONS_AL_MAX VAR_MAX + NUM_CONST_RANDOM_REGISTERS //3*K
#define CONS_IN_MIN CONS_AL_MAX + 1
#define CONS_IN_MAX CONS_AL_MAX + NUM_CONST_IN_REGISTERS
#define OP_MIN 1
#define OP_MAX 9
#define REGISTER_OFFSET CONS_AL_MAX + 1

/*REGISTROS

r[0] registro variable de salida - inicializado a 1
r[1] .. r[k] registros variables inicializados a 1
r[k + 1] .. r[2*k] registros aleatorios constantes
r[2*k + 1] .. r[3*k] registros de entrada constantes
*/

double R_OUT[NUM_OUT_REGISTERS];
double R_VAR[NUM_VAR_REGISTER];

#define INIT_VAR_VALUE 1.0 //Valor inicial de los registros variables

void init_parameters() {

	N = CONFIG.size();
	K = 0;
	for (int i = 0; i < N; i++) {
		if (CONFIG[i] == "1") {
			K++;
		}
	}

	R_OUT[0] = INIT_VAR_VALUE;

	for (int i = 0; i < NUM_VAR_REGISTER; i++) {
		R_VAR[i] = INIT_VAR_VALUE;
	}

}
/*
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


c_undef = 1.0
operations = {  1   : 'r_all[{0}] = r_all[{1}] + r_all[{2}]',
                16  : 'r_all[{0}] = r_all[{1}] + in_t[{2}]',
                2   : 'r_all[{0}] = r_all[{1}] - r_all[{2}]',
                32  : 'r_all[{0}] = r_all[{1}] - in_t[{2}]',
                3   : 'r_all[{0}] = r_all[{1}] * r_all[{2}]',
                48  : 'r_all[{0}] = r_all[{1}] * in_t[{2}]',
                4   : 'r_all[{0}] = (r_all[{1}] / r_all[{2}]) if r_all[{2}] != 0 else r_all[{1}] + Parameters.c_undef',
                64  : 'r_all[{0}] = (r_all[{1}] / in_t[{2}]) if in_t[{2}] != 0 else r_all[{1}] + Parameters.c_undef',
                5   : 'r_all[{0}] = r_all[{2}] ** 2',
                80  : 'r_all[{0}] = in_t[{2}] ** 2',
                6   : 'r_all[{0}] = math.log10(abs(r_all[{2}])) if r_all[{2}] != 0 else r_all[{2}] + Parameters.c_undef',
                96  : 'r_all[{0}] = math.log10(abs(in_t[{2}])) if in_t[{2}] != 0 else in_t[{2}] + Parameters.c_undef',
                7   : 'r_all[{0}] = math.sqrt(abs(r_all[{2}]))',
                112 : 'r_all[{0}] = math.sqrt(abs(in_t[{2}]))',
                8   : 'r_all[{0}] = math.sin(r_all[{2}])',
                128 : 'r_all[{0}] = math.sin(in_t[{2}])',
                9   : 'r_all[{0}] = math.cos(r_all[{2}])',
                144 : 'r_all[{0}] = math.cos(in_t[{2}])'
                }
*/


/************************************  ALGORITMO EVOLUTIVO ***********************************

    num_generations: cantidad de generaciones de individuos en el algoritmo evolutivo
    population_size: el tamaño de la población.
    demes: cantidad de subpoblaciones que se procesarán en paralelo.
    freq_stats: cada cuantas generaciones se imprimirá el estado del algoritmo.
    pool_size: cantidad de individuos qeu participaran del torneo
    migration_rate: porcentaje de individuos de la subpoblacion que van a migrar.

**********************************************************************************************/

#define NUM_GENERATIONS 128
#define POPULATION_SIZE 1024
#define DEMES 8
#define FREQ_STATS 10
#define POOL_SIZE 2
#define MIGRATION_RATE 0.06
#define GEN_TO_MIGRATE 32


/***************************************  PROBABILIDADES **************************************

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
*************************************************************************************************/

#define P_MIGRATION 0.80
#define P_MIGRATION_CRITERIA 0.3


#define P_REG_OP2_CONST 0.7
#define P_CONST_IN 0.75
#define CONST_MIN 0 //50
#define CONST_MAX 25 //50

#define P_MACRO_MUTATION 0.50
#define P_INS 0.75
#define P_DEL 0.25

#define P_MICRO_MUTATION 0.90
#define P_REGMUT 0.50
#define P_OPERMUT 0.45
#define P_CONSTMUT 0.05
#define STEP_SIZE_CONST 5

#define P_CROSSOVER 0.10


/*************************************  MULTIPROCESAMIENTO *************************************
Las operaciones sobre la población,como la inicialización y la evaluación de fitness se realiza
utilizando multiprocessing de Python
*/
#define NUM_PROCESSORS 4 //NÚMERO DE PROCESOS WORKERS
#define CHUNK_SIZE POPULATION_SIZE / DEMES // NUM_PROCESSORS

#define CHUNK_SIZE_STEP DEMES // NUM_PROCESSORS


/****************************************  CARGA DE DATOS ***************************************
                Carga de datos e inicialización de registros de entrada
*/



/*
INICIALIZACION DE REGISTROS DE ENTRADA CONSTANTES
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
data_samples = read_samples()
r_const = init_reg_in_const()
*/
