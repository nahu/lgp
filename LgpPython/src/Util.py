# -*- coding: utf-8 -*-
"""
Módulo con funciones de lectura de archivo de datos
entre otras

@authors:
- U{Nahuel Hernández<mailto:jnahuelhernandez@gmail.com>}
- U{Vanessa Cañete<mailto:vanessa.can.89@gmail.com>}

@since: 1.0
"""

import random
import Parameters

def sum_errors(list_of_errors):
    error_total = 0
    
    for x in list_of_errors:
        error_total += x
    list_of_errors.append(error_total)
    list_of_errors.append(error_total/Parameters.validation_lines)
    
    return list_of_errors



def errors_to_file(f_errors, final_table):
    f = open(f_errors, "w")

    for t in range(Parameters.validation_lines + 3):
        if t == Parameters.validation_lines:
            row = "Error total;"
        elif t ==  Parameters.validation_lines + 1:
            row = "Error promedio;"
        elif t ==  Parameters.validation_lines + 2:
            row = "Posicion;"
        else:
            row = str(Parameters.training_lines + t) + ";"
        for i in range(len(final_table)):
            row += (str(final_table[i][t]) + ";")
        row += "\n"
        f.write(row.replace('.', ','))
        
    f.close()

def estimations_to_file(f_estimations, final_table):
    f = open(f_estimations, "w")

    for t in range(Parameters.lines + 1):
        if t == 0:
            row = ";"
        else:
            row = str(t - 1) + ";"
            
        row += str(final_table[t]) + "\n"
        f.write(row.replace('.', ','))
        
    f.close()

def programs_to_file(f_programs, best_individuals):
    g = open(f_programs, "w")
    
    for b in best_individuals:
        g.write(repr(b))
        g.write("\n")
        g.write("#Effective Program:\n")
        g.write(b.get_program_in_python())
        g.write("\n\n\n")
        
    g.close()


def parameters_to_file(f_param):
    f = open(f_param, "w")
    
    '''
    ps =  [("f.write('" + a + "' + str(Parameters." + a + ") + @)") for a in dir(Parameters) \
            if (isinstance(Parameters.__dict__.get(a), types.FloatType) or \
               isinstance(Parameters.__dict__.get(a), types.IntType))]
    for l in ps:
    '''
    f.write("PARÁMETROS\n")
    f.write("----------------------------------------------------------------\n")
    f.write("--Problema\n")
    f.write('config\t' + Parameters.config + '\n')
    f.write('n\t' + str(Parameters.n) + '\n')
    f.write('k\t' + str(Parameters.k) + '\n')
    
    
    f.write("\n----Muestra de datos\n")
    f.write('lines\t' + str(Parameters.lines) + '\n')
    f.write('training_lines\t' + str(Parameters.training_lines) + '\n')
    f.write('validation_lines\t' + str(Parameters.validation_lines) + '\n')
    
    
    f.write("\n--Algoritmo Evolutivo\n")
    f.write('num_generations\t' + str(Parameters.num_generations) + '\n')
    f.write('pool_size\t' + str(Parameters.pool_size) + '\n')
    f.write('population_size\t' + str(Parameters.population_size) + '\n')
    f.write('demes\t' + str(Parameters.demes) + '\n')
    f.write('migration_rate\t' + str(Parameters.migration_rate) + '\n')
    f.write('gen_to_migrate\t' + str(Parameters.gen_to_migrate) + '\n')
    f.write('w_ob1\t' + str(Parameters.w_ob1) + '\n')
    f.write('w_ob2\t' + str(Parameters.w_ob2) + '\n')
    
    f.write("\n--Probabilidades\n")
    f.write('p_migration\t' + str(Parameters.p_migration) + '\n')
    f.write('p_migration_criteria\t' + str(Parameters.p_migration_criteria) + '\n')
    f.write("--Registros constantes\n")
    f.write('p_reg_op2_const\t' + str(Parameters.p_reg_op2_const) + '\n')
    f.write('p_const_in\t' + str(Parameters.p_const_in) + '\n')
    
    f.write("\n----Crossover\n")
    f.write('p_crossover\t' + str(Parameters.p_crossover) + '\n')

    f.write("\n----Mutación Macro\n")
    f.write('p_macro_mutation\t' + str(Parameters.p_macro_mutation) + '\n')
    f.write('p_del\t' + str(Parameters.p_del) + '\n')
    f.write('p_ins\t' + str(Parameters.p_ins) + '\n')

    f.write("\n----Mutación Micro\n")
    f.write('p_micro_mutation\t' + str(Parameters.p_micro_mutation) + '\n')
    f.write('p_opermut\t' + str(Parameters.p_opermut) + '\n')
    f.write('p_regmut\t' + str(Parameters.p_regmut) + '\n')
    f.write('p_constmut\t' + str(Parameters.p_constmut) + '\n')
    
    f.write("\n--Registros\n")
    f.write("\n----Cantidades\n")
    f.write('num_registers\t' + str(Parameters.num_registers) + '\n')
    f.write('   num_out_registers\t' + str(Parameters.num_out_registers) + '\n')
    f.write('num_var_register\t' + str(Parameters.num_var_register) + '\n')
    f.write('num_conts_registers\t' + str(Parameters.num_conts_registers) + '\n')
    f.write('   num_const_random_registers\t' + str(Parameters.num_const_random_registers) + '\n')
    f.write('   num_const_in_registers\t' + str(Parameters.num_const_in_registers) + '\n')
    
    f.write("\n--Valores en vector instrucción\n")
    f.write("\n----Operaciones\n")
    f.write('num_operators\t' + str(Parameters.num_operators) + '\n')
    f.write('op_min\t' + str(Parameters.op_min) + '\n')
    f.write('op_max\t' + str(Parameters.op_max) + '\n')
    
    f.write("\n----Salida\n")
    f.write('reg_out\t' + str(Parameters.reg_out) + '\n')
    
    f.write("\n----Variables\n")
    f.write('var_min\t' + str(Parameters.var_min) + '\n')
    f.write('var_max\t' + str(Parameters.var_max) + '\n')
    
    f.write("\n----Constantes aleatorias\n")
    f.write('cons_al_min\t' + str(Parameters.cons_al_min) + '\n')
    f.write('cons_al_max\t' + str(Parameters.cons_al_max) + '\n')
    
    f.write("\n------Valores\n")
    f.write('const_min\t' + str(Parameters.const_min) + '\n')
    f.write('const_max\t' + str(Parameters.const_max) + '\n')
    f.write('step_size_const\t' + str(Parameters.step_size_const) + '\n')
    
    f.write("\n----Constantes de entrada\n")
    f.write('cons_in_min\t' + str(Parameters.cons_in_min) + '\n')
    f.write('cons_in_max\t' + str(Parameters.cons_in_max) + '\n')
    
    
    f.write("\n--Instrucciones\n")
    f.write('num_ini_instructions\t' + str(Parameters.num_ini_instructions) + '\n')
    f.write('num_max_instructions\t' + str(Parameters.num_max_instructions) + '\n')
    f.write('num_min_instructions\t' + str(Parameters.num_min_instructions) + '\n')
    
    
    f.write("\n--Procesos\n")
    f.write('chunk_size\t' + str(Parameters.chunk_size) + '\n')
    f.write('num_processors\t' + str(Parameters.num_processors) + '\n')

    f.close()
    
    
def get_matrix_from_file(filename):
    _file = open(filename, 'r')
    """
    Formato del archivo: 
    Linea 1: columnas, 
    linea 2: filas, 
    Linea 3 en adelante: datos
    """
    
    n = _file.readline()
    T = _file.readline()
    
    matrix = []
    for i in range(int(T)): #T filas
        line=_file.readline().split(";");
        matrix.append(line[0:int(n)]) #n columnas
    
    
    _file.close()
    matrix = [map(float, i) for i in matrix]
    
    #Se retorna: cantidad de columnas, cantidad de filas, matriz de datos
    return(int(n), int(T), matrix)


def random_flip_coin(p):
    """ Returns True with the *p* probability. If the *p* is 1.0,
    the function will always return True, or if is 0.0, the
    function will return always False.

    :param p: probability, between 0.0 and 1.0
    :rtype: True or False

    """
    if p == 1.0: return True
    if p == 0.0: return False
    if random.random() <= p: return True
    else: return False


def write_csv_results(index_trafo, T):
    import csv, os
    
    lol = []
    '''para cada muestra se halla el error'''
    for t in range(0, T): 
        error = get_estimation_error(index_trafo, t)
        lol.append(error)
    file_writer = csv.writer(open(os.getcwd() + "python.csv","wb"), delimiter=';')
    muestra = -1
    for x in lol:
        muestra+=1
        file_writer.writerow([muestra, str(x).replace('.', ',')])


def get_estimation_error(index, t):
    return random.uniform(0,1000)


def create_data_file(data_file, n, t, exp):
    
    f = open(data_file, "w")
    
    #escribir las dos primeras lineas
    f.write(str(n))
    f.write("\n")
    f.write(str(t))
    f.write("\n")
    
    
    c_min = 2
    c_max = 20
    #los datos
    c = []
    
    for trafo in range(n):
        param = {}
        param["c1"] = random.uniform(c_min, c_max)
        param["c2"] = random.uniform(c_min, c_max)
        c.append(param)
        print "Trafo " + str(trafo) + "\n\t c1: " + str(param["c1"]) + ", c2: " + str(param["c2"]) + "\n" 
        
    for x in range(t):
        row = ""
        x_exp = x**exp
        
        for trafo in range(n):
            y = c[trafo]["c1"] + c[trafo]["c2"] * x_exp
            row += str(y)
            if trafo != (n - 1):
                row += ";"
        row += "\n"
        f.write(row)
    


if __name__ == "__main__":
    """
    #La tupla retornada tiene: (columnas, filas, matriz)
    
    _tuple = get_matrix_from_file("../data/Datos60.txt")
    
    print "Transformadores> "
    print _tuple [0]
    print "Muestras> "
    print _tuple [1]
    print "Datos>"
    for i in _tuple [2]:
        print i
    random_flip_coin(0.4)
    """ 
    
    data_file = "../data/Datos8-exp4.txt"
    n = 8
    t = 248
    exp = 4
    config = "10101011"
    
    create_data_file(data_file, n, t, exp)
    
    