# -*- coding: utf-8 -*-

"""
Módulo que define la clase de la Población dentro del LGP
y las funciones relacionadas

@authors:
- U{Nahuel Hernández<mailto:jnahuelhernandez@gmail.com>}
- U{Vanessa Cañete<mailto:vanessa.can.89@gmail.com>}

@since: 1.0
"""

import Individual
import Parameters

import sys

import Util

import random

'''
deprecade
'''
def tournament(competitors):
    choosen = competitors[0]
    
    for i in range(1, len(competitors)):
        if choosen.evaluate() < competitors[i].evaluate():
            choosen = competitors[i]

    return choosen


def check_out_register(individual):
    return individual.genomeList[individual.height - 1][1] == 0
    
    
def tournament_with_mutation(competitors):
    choosen = competitors[0]
    
    for i in range(1, len(competitors)):
        if Individual.compare(choosen, competitors[i]) == -1:
            choosen = competitors[i]
        
    '''
    Se hacen copias temporales para reemplazar luego a los perdedores del torneo
    '''
    winner = choosen.clone()
    
    '''
    Se realizan la macro y micro mutación según probabilidades
    '''
    try:
        if Util.random_flip_coin(Parameters.p_macro_mutation):
            winner = macro_mutation(winner)
    
        if not check_out_register(winner):
            print "La macro mató"
    except:
        print "La macro matOOOó"
    
    try:
        if Util.random_flip_coin(Parameters.p_micro_mutation):
            winner = micro_mutation(winner)
           
        if not check_out_register(winner):
            print "La miiicro mató"
    except:
        print "La miiicro mató"
        
    return list([winner, choosen])


def crossover(genome1, genome2):
    sister = None
    brother = None
    cuts_points_mom = [None, None]
    cuts_points_dad = [None, None]
    
    if genome1.height > Parameters.num_max_instructions:
        print "genome 1 - superado el número maximo de instrucciones ENTRADA CROSS"
    
    if genome2.height > Parameters.num_max_instructions:
        print "genome 2 - superado el número maximo de instrucciones ENTRADA CROSS"
        
    if genome1.height < Parameters.num_min_instructions:
        print "genome 1 - superado el número minimo de instrucciones ENTRADA CROSS"
    
    if genome2.height < Parameters.num_min_instructions:
        print "genome 2 - superado el número minimo de instrucciones ENTRADA CROSS"
        
    mom, dad = (genome1, genome2) if genome1.height < genome2.height else (genome2, genome1)
    try:
        max_segment_size_mom = mom.height - 1 #se puede cruzar todo menos la última instrucción
        mom_segment_size = random.randint(1, max_segment_size_mom) #por lo menos una instrucción se tiene que cruzar
        max_padding_mom =  mom.height - mom_segment_size - 1
        cuts_points_mom[0] = random.randint(0, max_padding_mom) #desde donde se puede comenzar para que alcancen las instrucciones del segmento a cruzar
        cuts_points_mom[1] = cuts_points_mom[0] + mom_segment_size
        
        #cuts_points_mom = [0, mom.height - 2]#[random.randint(1, mom.height - 2), random.randint(1, mom.height - 2)]
       
        #el máximo segmento a cruzar es lo que le falta al mom para completar el máximo número de instrucciones permitidas
        max_segment_full_mom = Parameters.num_max_instructions - (mom.height - mom_segment_size) 
        max_segment_num_min_in_dad = (dad.height + mom_segment_size) - Parameters.num_min_instructions
        
        #se elije el menor de los máximos
        max_segment_size_dad = max_segment_full_mom if max_segment_full_mom < max_segment_num_min_in_dad else max_segment_num_min_in_dad
        #si el maximo es mayor a lo longitud del padre, se elige la longitud como máximo
        max_segment_size_dad = (dad.height - 1) if (dad.height - 1) < max_segment_size_dad else max_segment_size_dad
        
        #lo que hay que quitarle para que quede en máximo número de instrucciones
        min_segment_full_dad = (dad.height + mom_segment_size) - Parameters.num_max_instructions
        
        # lo que falta para completar el mínimo número de instrucciones al mom
        mim_segment_size_num_min_in_mom = Parameters.num_min_instructions - (mom.height - mom_segment_size)
         
        mim_segment_size_dad = min_segment_full_dad if (min_segment_full_dad > mim_segment_size_num_min_in_mom) else mim_segment_size_num_min_in_mom
        mim_segment_size_dad = 1 if mim_segment_size_dad <= 0 else mim_segment_size_dad
        
        dad_segment_size = random.randint(mim_segment_size_dad, max_segment_size_dad)
        
        max_padding_dad = dad.height - dad_segment_size - 1
        cuts_points_dad[0] = random.randint(0, max_padding_dad) #desde donde se puede comenzar para que alcancen las instrucciones del segmento a cruzar
        cuts_points_dad[1] = cuts_points_dad[0] + dad_segment_size
        
        
        sister = mom.clone()
        sister.genomeList[cuts_points_mom[0]:cuts_points_mom[1]] = dad.genomeList[cuts_points_dad[0]:cuts_points_dad[1]]
    
        brother = dad.clone()
        brother.genomeList[cuts_points_dad[0]:cuts_points_dad[1]] = mom.genomeList[cuts_points_mom[0]:cuts_points_mom[1]]
        
        
        
        sister.height = len(sister.genomeList)
        sister.index = genome1.index
        sister.set_altered()
        brother.height = len(brother.genomeList)
        brother.index = genome2.index
        brother.set_altered()
        
        if sister.height > Parameters.num_max_instructions:
            print "sister  - superado el número maximo de instrucciones ENTRADA CROSS"
        
        if brother.height > Parameters.num_max_instructions:
            print "brother- superado el número maximo de instrucciones ENTRADA CROSS"
            
        if sister.height < Parameters.num_min_instructions:
            print "sister 1 - superado el número minimo de instrucciones ENTRADA CROSS"
        
        if brother.height < Parameters.num_min_instructions:
            print "brother 2 - superado el número minimo de instrucciones ENTRADA CROSS"
            
    except Exception as e:
        print "oh no!"
        print e
        print sys.exc_info()[0]
        
    
    return list([sister, brother])


def impresiones_mutacion(ins, mutpoint, eff):
    if ins:
        print "Insercion de instrucciones en el punto " + str(mutpoint)
        print "Registros efectivos al punto de mutacion"
        print eff
    else:
        print "Borrado de instrucciones en el punto " + str(mutpoint)


def macro_mutation(genome):
    """Agrega o quita instrucciones
     -- Alg. 6.1 -- 
     p_ins > p_del 
     """
    insertion = Util.random_flip_coin(Parameters.p_ins)
    mutation_point = random.randint(0, genome.height - 2)
        
    if genome.height < Parameters.num_max_instructions and \
    (insertion or genome.height == Parameters.num_min_instructions):
        new_instruction = Individual.create_new_instruction()
        reg_eff, to_mutate = genome.get_effective_registers(mutation_point)
        #impresiones_mutacion(insertion, mutation_point, reg_eff)
        
        while not reg_eff: 
            """
            se da en el caso de que el punto de mutación esté por debajo de la última
            instrucción efectiva 
            y ésta sea unaria con un operando constante
            """
            #cambiar el registro constante del operador unario por uno variable
            genome.genomeList[to_mutate][3] = random.randint(Parameters.var_min, Parameters.var_max)
            reg_eff, to_mutate = genome.get_effective_registers(mutation_point)
        
        new_instruction[1] = random.choice(reg_eff)
        genome.genomeList.insert(mutation_point, new_instruction)
        genome.height += 1
        
        
    if genome.height > Parameters.num_min_instructions and \
    (not insertion or genome.height == Parameters.num_max_instructions):
        del genome.genomeList[mutation_point]
        genome.height -= 1
        #impresiones_mutacion(insertion,mutation_point,[])
    
    genome.set_altered()
    
    if genome.height > Parameters.num_max_instructions:
        print "superado el número maximo de instrucciones MACRO"
        print genome.height > Parameters.num_max_instructions
    return genome


def micro_mutation(genome):
    """Muta las instrucciones efectivas internamente
    p_regmut = 0.5 #probabilidad de mutar un registro 
    p_opermut = 0.25 #probabilidad de mutar una
    p_constmut = 0.25
    """
    eff, indices = genome.get_effective_instructions_with_indices()
    index = random.randint(0, len(eff) - 1)
    #print "index " + str(index)
    instruction = eff[index]
    mutation_point = indices[index]
    #print "mutation point: " + str(mutation_point) + " index : " + str(index)
    
    type = select_micro_mutacion_type(random.random())

    if (type == "constantes"):
        constants_indices = genome.get_effective_constant_indices()
        #print constants_indices
        if constants_indices:
            ins_with_constant_index = random.choice(constants_indices)
            register_mutation_index = genome.genomeList[ins_with_constant_index][3]
            genome.r_all[register_mutation_index] += ((-1)**(random.randint(0, 1)) * random.uniform(0, Parameters.step_size_const))
            
        else: #no hay instrucciones efectivas con registros constantes variables
            type = select_micro_mutacion_type(random.random()) #si vuelve a salir constante, se elige mutación de registro o operaciones con igual probabilidad
            if (type == "constantes"):
                type = "registros" if Util.random_flip_coin(0.5) else "operaciones"
                
                
    if (type == "registros"):
        if len(eff) == 1: #una sola instrucción efectiva, no se puede cambiar r[0]
            if (instruction[0] < 5):
                pos_to_replace = random.randint(2, 3)
            else: #operación unaria, cambiar el segundo operando
                pos_to_replace = 3
        else:
            if (instruction[0] < 5):
                pos_to_replace = random.randint(1, 3)
            else: #operación unaria, cambiar el segundo operando o el destino
                pos_to_replace = 1 if Util.random_flip_coin(0.5) else 3
        
        if pos_to_replace == 1:

            if (index + 1) < len(indices):
                reg_eff, to_mutate = genome.get_effective_registers(indices[index + 1])
                
                if reg_eff:
                    reg_eff.remove(instruction[1]) #remover el registro destino de los registros efectivos
                    
                op, pos_to_replace = Individual.get_random_register(pos_to_replace, reg_eff, instruction)
            else: #el punto de mutación es la última instrucción con el r[0]
                if (instruction[0] < 5): 
                    pos_to_replace = random.randint(2, 3)
                else: #operación unaria, cambiar el segundo operando
                    pos_to_replace = 3
                    
                op, pos_to_replace = Individual.get_random_register(pos_to_replace)
        else: #para los casos de operandos 1 y 2
            op, pos_to_replace = Individual.get_random_register(pos_to_replace)
            
        instruction[pos_to_replace] = op
            
        genome.genomeList[mutation_point] = instruction
        
    if (type == "operaciones"):
        diff_op = random.randint(Parameters.op_min, Parameters.op_max)
        
        while instruction[0] == diff_op:
            diff_op = random.randint(Parameters.op_min, Parameters.op_max)
            
        instruction[0] = diff_op
        genome.genomeList[mutation_point] = instruction
    
    genome.set_altered()
    return genome

def select_micro_mutacion_type(prob):
    if prob <= Parameters.p_regmut:
        return "registros"
    elif prob > Parameters.p_regmut and prob <= Parameters.p_opermut + Parameters.p_regmut:
        return "operaciones"
    elif prob > (Parameters.p_opermut + Parameters.p_regmut) and prob <= (Parameters.p_opermut + Parameters.p_regmut + Parameters.p_constmut):
        return "constantes"

def best_training_in_pop(population):
    return population.best_training()

def best_validation_in_pop(population):
    return population.best_validation()

class Population:
    def __init__(self, size, config_position):
        self.internal_pop = []
        self.pop_size = size
        self.config_position = config_position
        self.create()

    def create(self, **args):
        for i in xrange(self.pop_size):
            self.internal_pop.append(Individual.Individual(4, i, self.config_position))


    def indices_selection(self, pool_size):
        index_pool = []
        
        while len(index_pool) < pool_size:
            index = random.randint(0, self.pop_size - 1)
            if not (index in index_pool):
                index_pool.append(index)
                
        return index_pool

    
    def selection_from_indices(self, indices):
        pool = []
        
        for i in indices:
            pool.append(self.internal_pop[i])
                
        return pool


    def best_training(self):
        best = self.internal_pop[0]
        
        for i in range(1, self.pop_size):
            if (Individual.compare(self.internal_pop[i], best) == 1):
                best = self.internal_pop[i]
        
        return best
    
    def best_validation(self):
        '''
        Se establece el atributo validation error de los individuos de la población
        '''
        for i in self.internal_pop:
            i.eval_validation()
            
        best = self.internal_pop[0]
        
        '''
        Se busca el de menor error
        '''
        for i in range(1, self.pop_size):
            if (Individual.compare_error(self.internal_pop[i], best) == -1):
                best = self.internal_pop[i]
        
        return best
    
if __name__ == "__main__":
    pool = Pool(processes=Parameters.num_processors)
    population = Population(2, pool, Parameters.index_to_predict)
    population.initialize()
    genome1 = population.internal_pop[0]
    genome2 = population.internal_pop[1]
    print "Instruciones originales"
    print "ind 1"
    print genome1
    print "ind 2"
    print genome2
    
    print "Effectivas antes"
    print "program 1"
    print genome1.get_program_in_python()
    print "program 2"
    print genome2.get_program_in_python()
    
    
    sister, brother = crossover(genome1, genome2)
    print "Instruciones cruzadas"
    print "el 1"
    print sister
    print "el 2"
    print brother
    '''
    print "Instruciones originales"
    print "el 1"
    print genome1
    print "el 2"
    print genome2
    '''
    print "Effectivas después"
    print "program 1"
    print sister.get_program_in_python()
    print "program 2"
    print brother.get_program_in_python()
    
    
    pool.close() 
    pool.join()
    