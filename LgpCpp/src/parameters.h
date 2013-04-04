/*
 * parameters.h
 *
 *  Created on: Mar 16, 2013
 *      Author: Vanessa Cañete, Nahuel Hernández
 */

#define _USE_MATH_DEFINES

#define FILE_NAME "./data/Datos60.txt"
//Solo faltan 5 transformadores. [1,7,15,23,39]
std::string CONFIG = "1011111011111110111111101111111111111110";
#define N 40
#define K 35

#define INDEX_TO_PREDICT 1


/******************************* DATOS **********************************/

#define LINES 248
#define TRAINING_LINES 200
#define VALIDATION_LINES 48

#define W_OB1 1 //error
#define W_OB2 1 //desviacion


/*INSTRUCCIONES

Cada individuo posee una lista de instrucciones
Cada instrucción se representa como una lista de 4 enteros como sigue
[operación, destino, operador1, operador2]

la convención para asegurar que el programa tenga una salida es que la
última instrucción sea del tipo [operación, 0, operador1, operador2]
*/

#define NUM_MIN_INSTRUCTIONS (30 * K)
#define NUM_MAX_INSTRUCTIONS (12 * K)
#define NUM_INI_INSTRUCTIONS (6 * K)
#define NUM_OPERATORS 9


#define NUM_VAR_REGISTER 9 //K
#define NUM_CONST_IN_REGISTERS K
#define NUM_OUT_REGISTERS 1
#define NUM_CONST_RANDOM_REGISTERS 6//K
#define NUM_CONST_MATH_REGISTERS 4
#define NUM_CONTS_REGISTERS (NUM_CONST_RANDOM_REGISTERS + NUM_CONST_IN_REGISTERS + NUM_CONST_MATH_REGISTERS)
#define NUM_REGISTERS (NUM_CONTS_REGISTERS + NUM_VAR_REGISTER + NUM_OUT_REGISTERS)
#define NUM_INDIVIDUAL_REGISTERS (NUM_REGISTERS - NUM_CONST_IN_REGISTERS)

//Límites en las instrucciones
#define REG_OUT 0
#define VAR_MIN 1
#define VAR_MAX NUM_VAR_REGISTER
#define CONST_AL_MIN (VAR_MAX + 1)
#define CONST_AL_MAX (VAR_MAX + NUM_CONST_RANDOM_REGISTERS + NUM_CONST_MATH_REGISTERS) //3*K //aca le agrego las constantes matematicas
#define CONST_IN_MIN (CONST_AL_MAX + 1)
#define CONST_IN_MAX (CONST_AL_MAX + NUM_CONST_IN_REGISTERS)
#define OP_MIN 1
#define OP_MAX 9

#define REGISTER_OFFSET (CONST_AL_MAX + 1)

/*REGISTROS

r[0] registro variable de salida - inicializado a 1
r[1] .. r[k] registros variables inicializados a 1
r[k + 1] .. r[2*k] registros aleatorios constantes
r[2*k + 1] .. r[3*k] registros de entrada constantes
*/

#define INIT_VAR_VALUE 1.0 //Valor inicial de los registros variables


/************************************ OPERACIONES ************************************/
#define ADD 1
#define SUB 2
#define MUL 3
#define DIV 4
#define POW2 5
#define LOG10 6
#define SQRT 7
#define SIN 8
#define COS 9

#define C_UNDEF 1.0

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

#define P_REG_OP1_CONST 0.5
#define P_REG_OP2_CONST 0.1
#define P_CONST_IN 0.75
#define CONST_MIN 0 //50
#define CONST_MAX 1 //50

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
#define CHUNK_SIZE (POPULATION_SIZE / DEMES) // NUM_PROCESSORS

#define CHUNK_SIZE_STEP (DEMES / NUM_PROCESSORS)


