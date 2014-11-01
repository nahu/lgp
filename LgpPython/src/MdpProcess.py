# -*- coding: utf-8 -*-

"""
Prueba de reduccion de dimensinalidad

@authors:
- U{Nahuel Hernández<mailto:jnahuelhernandez@gmail.com>}
- U{Vanessa Cañete<mailto:vanessa.can.89@gmail.com>}

"""

from Util import get_matrix_from_file
import Parameters
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.mlab as mlab
import matplotlib.cbook as cbook
import matplotlib.ticker as ticker
import mdp
import csv
import os
import copy

class MdpProcess():
    def __init__(self):
        print "Constructor"
        
    def process(self):
        filename = "../data/Datos60.txt"
        col, fil, data = get_matrix_from_file(filename)
        data = zip(*data)
        
        ceros = {}
        for i in range(Parameters.n -1 , 0, -1):
            if Parameters.config[i] == '0':
                print ">> Borrando..." + str(i)
                ceros[i] = copy.copy(data[i])
                del data[i]
                
        print ceros
        
        data = zip(*data)
        
        data_t = data[:200]
        data_v = data[200:]
        
        #data_t = zip(*data_t)
        #data_v = zip(*data_v)
        
        print ">> Creando array de numpy"
        x_t = np.array(data_t, float)
        x_v = np.array(data_v, float)
        
        print ">> ENTRENAMIENTO" 
        print ">> Se tienen " + str(x_t.shape[1]) + " variables, " + str(x_t.shape[0]) + " observaciones"
        
        
        print ">> VALIDACAION" 
        print ">> Se tienen " + str(x_v.shape[1]) + " variables, " + str(x_v.shape[0]) + " observaciones"
        
        out_dim = 5
        in_dim = 35

        pcanode1 = mdp.nodes.PCANode(input_dim=in_dim, output_dim=out_dim, dtype='float64', reduce=True)
        #pcanode1 = mdp.nodes.PCANode()
        #pcanode1.reduce = Tr
        
        print pcanode1
        
        print ">> Entrenando a x_t..."
        pcanode1.train(x_t)

        print ">> Fin del entrenamiento..."
        pcanode1.stop_training()
        
        print ">> Dimension de salida..."
        print pcanode1.output_dim 
        
        print ">> Explained variance..."
        print pcanode1.explained_variance
        
        print ">> Extracting avg"
        print pcanode1.avg 
        
        #print ">> Extracting projection matrix"
        #v = pcanode1.get_projmatrix()
        #print v
        
        y_t = pcanode1.execute(x_t) #Es lo mismo que pcanode1.execute(x)
        
        print ">> Resultado Entrenamiento..."
        print y_t
        print ">> Se tienen " + str(y_t.shape[1]) + " variables, " + str(y_t.shape[0]) + " observaciones"
        
        
        y_v = pcanode1.execute(x_v) #Es lo mismo que pcanode1.execute(x)
        
        print ">> Resultado Validacion..."
        print y_v
        print ">> Se tienen " + str(y_v.shape[1]) + " variables, " + str(y_v.shape[0]) + " observaciones"

        print ">> Guardando en archivo... ../data/datos_reducidos.txt "
        lol_t = y_t.tolist()
        lol_v = y_v.tolist()
        lol = lol_t + lol_v

        file = "../data/datos_reducidos.csv"
        f = open(file, "w")
        
        f.write(str(out_dim + 1) + '\n')
        f.write(str(fil) + '\n')
        
        for t in range(len(lol)):#248
            row = ""
            row = str(ceros[Parameters.index_to_predict][t]) + ";"
            for i in range(len(lol[t])):#10
                row += (str(lol[t][i]) + ";")
            row += "\n"
            f.write(row)#f.write(row.replace('.', ','))
            
        f.close()  
        print "Se termino de escribir el archivo"
        
        
    def plot_one_data_series(self, order):
        filename = "../data/Datos60.txt"
        col, fil, data = get_matrix_from_file(filename)

        #data = zip(*data)
        
        data_t = data[:200]
        data_v = data[200:]
        
        data_t = zip(*data_t)
        data_v = zip(*data_v)
        
        print ">> Creando array de numpy"
        x_t = np.array(data_t, float)
        x_v = np.array(data_v, float)


        fig = plt.figure()
        ax = fig.add_subplot(111)
        ax.plot(x_t[order], 'o-')
        ax.plot(range(200, 200 + len(x_v[order])), x_v[order], 'go-')


    def get_discrete_values(self):
        filename = "../data/Datos60.txt"
        col, fil, data = get_matrix_from_file(filename)

        #data = zip(*data)
        
        data_t = data[:200]
        data_v = data[200:]
        
        #data_t = zip(*data_t)
        #data_v = zip(*data_v)
        data_t.sort()
        data_v.sort()
        
        print "Valores en entrenamiento"
        valores = set([])
        
        
        for fila in data_t:
            for valor in fila:
                valores.add(valor)
        

        print sorted(valores)
        print "cantidad: ", str(len(valores))
        
        print "Valores en validación"
        valores = set([])
        
        
        for fila in data_v:
            for valor in fila:
                valores.add(valor)
        
        
        
        print sorted(valores)
        print "cantidad: ", str(len(valores))
        
        print "Valores en todo"
        valores = set([])
        
        
        for fila in data:
            for valor in fila:
                valores.add(valor)
        
        
        
        print sorted(valores)
        print "cantidad: ", str(len(valores))


if __name__ == "__main__":
    m = MdpProcess()
    m.process()
    """
    salida = []
    for i in range (40) :
        print "Transformador ", str(i)
        m.plot_one_data_series(i)
        res = raw_input("res: ")
        salida.append(res)
    
    print "Resultado"
    print salida
    """
    m.get_discrete_values()
        
        
