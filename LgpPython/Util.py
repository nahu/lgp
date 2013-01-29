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


def list_swap_element(lst, indexa, indexb):
    """ Swaps elements A and B in a list.

    Example:
        >>> l = [1, 2, 3]
        >>> Util.listSwapElement(l, 1, 2)
        >>> l
        [1, 3, 2]

    :param lst: the list
    :param indexa: the swap element A
    :param indexb: the swap element B
    :rtype: None

    """
    temp = lst[indexa]
    lst[indexa] = lst[indexb]
    lst[indexb] = temp
    
    
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


if __name__ == "__main__":
    """
    La tupla retornada tiene: (columnas, filas, matriz)
    """
    _tuple = get_matrix_from_file("Datos60.txt")
    
    print "Transformadores> "
    print _tuple [0]
    print "Muestras> "
    print _tuple [1]
    print "Datos>"
    for i in _tuple [2]:
        print i
    random_flip_coin(0.4)
    