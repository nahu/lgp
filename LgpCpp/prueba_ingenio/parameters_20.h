/*
 * parameters_base.h
 *
 *  Created on: Mar 16, 2013
 *      Author: Vanessa Cañete, Nahuel Hernández
 */

#define _USE_MATH_DEFINES

#define FILE_ANALISIS "./analisis-link/analisis.csv"
#define FILE_PROBABILIDADES "./analisis-link/probabilidades.csv"
#define FILE_NAME_ERRORES_X_DEME_TRAINING "errores_x_deme_training.csv"
#define FILE_NAME_ERRORES_X_DEME_VALIDATION "errores_x_deme_validation.csv"
#define FILE_NAME "./data/prueba_incremento/datos_2974_factor_03.csv"
//Solo faltan 5 transformadores. [1,7,15,23,39]
std::string CONFIG = "1011111011111110111111101111111111111110";

#define CONFIG_POSITION 1 //el transformador dentro de la configuración a encontrar


//#define N 8
#define N 40

//#define K 5
#define K 35


#define N_K (N - K)
//#define Q 10
#define DELTA 5


#define CNT_PRUEBAS 3

/*condiciones de parada */
#define ERROR_TO_STOP 45
#define NUM_MAX_GENERATION 1000


/******************************* DATOS **********************************/
#define LINES 2974
#define TRAINING_LINES 2500
#define VALIDATION_LINES 474
#define TRAINING 0
#define VALIDATION 1

#define W_OB1 1 //error
#define W_OB2 0 //desviacion

//para la función de evaluación de individuos
#define FITNESS 0
#define OB1 1
#define OB2 2

//para el operador de reproduccion, posibles tipos de estados despues de participar del torneo
#define NONE 0
#define WINNER 1
#define REPLACE 2


/*INSTRUCCIONES

Cada individuo posee una lista de instrucciones
Cada instrucción se representa como una lista de 4 enteros como sigue
[operación, destino, operador1, operador2]

la convención para asegurar que el programa tenga una salida es que la
última instrucción sea del tipo [operación, 0, operador1, operador2]
*/


#define NUM_MIN_INSTRUCTIONS (3 * K)
#define NUM_MAX_INSTRUCTIONS (10 * K)
#define NUM_INI_INSTRUCTIONS (6 * K)
#define NUM_OPERATORS 9


#define NUM_VAR_REGISTER 9//K

#ifdef Q
	#define NUM_CONST_IN_REGISTERS (Q + DELTA)
#else
	#define NUM_CONST_IN_REGISTERS (K + DELTA)
#endif

#define NUM_OUT_REGISTERS 1
#define NUM_CONST_RANDOM_REGISTERS 4//K
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
#define CONST_IN_MAX (CONST_AL_MAX + NUM_CONST_IN_REGISTERS - DELTA)
#define CONST_IN_DELTA_MIN (CONST_IN_MAX + 1)
#define CONST_IN_DELTA_MAX (CONST_IN_MAX + DELTA)
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
#define HUGE_NUMBER 9999999999

//Tipos de micromutaciones
#define CONSTANTES 0
#define REGISTROS 1
#define OPERACIONES 2

//Posiciones en una instruccion
#define OPERATION 0
#define DEST 1
#define OPERAND_1 2
#define OPERAND_2 3


/************************************  ALGORITMO EVOLUTIVO ***********************************

    num_generations: cantidad de generaciones de individuos en el algoritmo evolutivo
    population_size: el tamaño de la población.
    demes: cantidad de subpoblaciones que se procesarán en paralelo.
    freq_stats: cada cuantas generaciones se imprimirá el estado del algoritmo.
    pool_size: cantidad de individuos qeu participaran del torneo
    migration_rate: porcentaje de individuos de la subpoblacion que van a migrar.

**********************************************************************************************/



#define NUM_GENERATIONS 3
#define POPULATION_SIZE 416000//524288 //262144 //18
#define DEMES 16
#define FREQ_STATS 100
#define POOL_SIZE 8//32
#define POOL_REPRODUCTION 2 //4
#define MIGRATION_RATE 0.01
#define GEN_TO_MIGRATE 100 //250

#define CANT_ESTANCAMIENTO 10
#define ERROR_STEP 50
#define MIN_ERROR_STEP 1

#define TOURNAMENTS 6 //número par
#define P_DIVERSITY 0.5


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


#define P_MIGRATION 0.8
#define P_MIGRATION_CRITERIA 1


#define P_REG_OP1_CONST 0.4
#define P_REG_OP2_CONST 0.6
#define P_REG_CONST_DELTA 0.7
#define P_CONST_IN 0.80
#define CONST_MIN 0 //50
#define CONST_MAX 1 //50


#define P_MACRO_MUTATION 0.6//0.50
#define P_INS 0.51
#define P_DEL 0.49


#define P_MICRO_MUTATION 0.9//0.90
#define P_REGMUT 0.25
#define P_OPERMUT 0.7
#define P_CONSTMUT 0.05

#define STEP_SIZE_CONST 0.1


#define P_CROSSOVER 0.8//0.10



/*************************************  MULTIPROCESAMIENTO *************************************

Las operaciones sobre la población,como la inicialización y la evaluación de fitness se realiza
utilizando multiprocessing de Python

*/


#define NUM_PROCESSORS 4 //NÚMERO DE PROCESOS WORKERS
#define WHILE_BLOCK 1000 //para evitar whiles infinitos, por si pudiera llegarse a dar
