# -*- coding: utf-8 -*-
class Reader:

    def get_matrix_from_file(self, filename):
        _file = open(filename,'r')
        #Formato del archivo: Linea 1: columnas, linea 2: filas, Linea 3 en adelante: datos        n = _file.readline()
        n = _file.readline()
        T = _file.readline()
        matrix = []
        for i in range(int(T)): #T filas
            line=_file.readline().split(";");
            matrix.append(line[0:int(n)]) #n columnas
        _file.close()
        matrix = [map(int,i) for i in matrix ]
        #Se retorna: cantidad de columnas, cantidad de filas, matriz de datos
        return(int(n), int(T), matrix) 

if __name__ == "__main__":
    r = Reader()
    """La tupla retornada tiene: (columnas, filas, matriz)"""
    _tuple = r.get_matrix_from_file("Datos60.txt")
    
    print "Transformadores> "
    print _tuple [0]
    print "Muestras> "
    print _tuple [1]
    print "Datos>"
    for i in _tuple [2]:
        print i
    