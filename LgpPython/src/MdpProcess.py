# -*- coding: utf-8 -*-

"""
Prueba de reduccion de dimensinalidad

@authors:
- U{Nahuel Hernández<mailto:jnahuelhernandez@gmail.com>}
- U{Vanessa Cañete<mailto:vanessa.can.89@gmail.com>}

"""
import numpy as np
import mdp
import Util
import Parameters

class MdpProcess():
    def __init__(self):
        print "Constructor"
    def process(self):
        filename = "../data/Datos60.txt"
        col, fil, data = Util.get_matrix_from_file(filename)
        data = zip(*data)
        for i in range(Parameters.n -1 , 0, -1):
            if Parameters.config[i] == '0':
                print ">> Borrando..." + str(i)
                del data[i]
                
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
        
        pcanode1 = mdp.nodes.PCANode(input_dim=35, output_dim=10, dtype='float64', reduce=True)
        #pcanode1 = mdp.nodes.PCANode()
        #pcanode1.reduce = True
        
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
        
#To do: Se tendria que pasar a train(x) los datos de entrenamiento (200 muestras) y a 
#execute los datos restantes (48 muestras)....  
if __name__ == "__main__":
    m = MdpProcess()
    m.process()
        
        