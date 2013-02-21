/**
 * SCICTD.java
 *
 * @author Gustavo Planas, Esteban Benitez
 * @version 1.0
 */

package prueba;
import java.io.*;
import java.util.ArrayList;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * Class representing a SCICTD (Sistema de Caracterizacion Inteligente de la
 * Carga de Transformadores de Distribucion) problem.
 */
public class SCICTD {
	public static final String DATA_FOLDER = SCICTD.class.getResource( "/" ).getPath()  +
            ".." + System.getProperty( "file.separator" ) +
            "data" + System.getProperty( "file.separator" );
	
    public static int numberOfTrafos_;
    public int numberOfMuestras_;
    private double[][] matrizConsumo_;
    public int[] nodos = new int[numberOfTrafos_];
    public int training_lines = 200;
    public int validation_lines = 48;
    public Double [][] resultados = null;
    public ArrayList<double []> aij;
    public int k;
    public int n;
    public String configuracion = "";
    
    public SCICTD(int cantidadTrafos, int numeroDeObjetivos) throws FileNotFoundException, IOException, ClassNotFoundException {
        numberOfTrafos_ = cantidadTrafos;
        matrizConsumo_ = readProblem( DATA_FOLDER + "Datos60.txt", matrizConsumo_);
//        
//        solutionType_ = new BinarySolutionType(this);
//        variableType_ = new Class[numberOfVariables_];
//        length_ = new int[1];
//        variableType_[0] = Class.forName("jmetal.encodings.variable.Binary");
//        length_[0] = numberOfTrafos_;
    } // TSP

    public Double [][] evaluate(int[] nodos) {
    	System.out.println("Evaluando...");
         aij = new ArrayList(5);
        // Se crea el vector fitness de dos elementos: uno para cada objetivo.
        double[] fitness = new double[6];

        // CALCULO DEL FITNESS (Error promedio)

        // PASO 0: Obtener datos del individuo.
        k = 0; // Cantidad de medidores.
        for (int i = 0; i < nodos.length; i++) {
            if (nodos[i] == 1) {
                nodos[i] = 1;
                k++;
            }
        }

        n = numberOfTrafos_; // Cantidad de transformadores.
        int T = numberOfMuestras_; // Cantidad de muestras.
        double[][] mediciones = matrizConsumo_;
        resultados = new Double[T+2][n];
        // PASO 1: Generar derivadas de errores (Sistemas de ecuaciones).
        // Se guardan las posiciones de los medidores en el vector posiciones.
        int[] posiciones = new int[k];
        int pos = 0;
        for (int i = 0; i < n; i++) {
            if (nodos[i] == 1) {
                posiciones[pos] = i;
                pos++;
            }
            
        }
        // Se busca el valor desconocido, es decir, el Transformador que no tiene asignado medidor.
        fitness[0] = 0;
        int desconocido = 0;
        for (int d = 0; d < n; d++) {
            // Se calculan los errores para cada transformador que no tiene medidor.

            if (nodos[d] == 0) { //Si no hay medidor
            	System.out.println("Transformador..." + d);

                // Matriz de ecuaciones inicializada a ceros.
                double[][] ecuaciones = new double[k][k + 1];
                for (int i = 0; i < k; i++) {
                    for (int j = 0; j < k + 1; j++) {
                        ecuaciones[i][j] = 0;
                    }
                }
                // Vector de factores aij inicializada a ceros.
                double[] factores = new double[k];
                for (int i = 0; i < k; i++) {
                    factores[i] = 0;
                }
                desconocido = d;
                // Se arman las ecuaciones.
                int pivot;
                for (int i = 0; i < k; i++) {
                    pivot = posiciones[i];
                    for (int j = 0; j < k + 1; j++) {
                        for (int l = 0; l < training_lines; l++) { 
                            if (j == k) {
                                ecuaciones[i][j] += mediciones[l][pivot] * mediciones[l][desconocido];
                            } else {
                                ecuaciones[i][j] += mediciones[l][pivot] * mediciones[l][posiciones[j]];
                            }
                        }
                    }
                }
                // PASO 2: Resolver el Sistema de ecuaciones por el metodo de Gauss.
                // Escalonar la matriz de ecuaciones.
                int col = k + 1;
                double factor;
                for (int f = 0; f < k - 1; f++) {
                    factor = ecuaciones[f][f];
                    for (int j = 0; j < col; j++) {
                        ecuaciones[f][j] = ecuaciones[f][j] / factor;
                    }
                    for (int i = f + 1; i < k; i++) {
                        factor = ecuaciones[i][f] / ecuaciones[f][f];
                        for (int j = f; j < col; j++) {
                            ecuaciones[i][j] = ecuaciones[i][j] - (factor * ecuaciones[f][j]);
                        }
                    }
                }
                int f = k - 1;
                factor = ecuaciones[f][f];
                for (int j = 0; j < col; j++) {
                    ecuaciones[f][j] = ecuaciones[f][j] / factor;
                }
                // Reemplazar los valores hallados en la matriz para encontrar los factores.
                int fac = f;
                for (int i = f; i >= 0; i--) {
                    factores[i] = ecuaciones[i][k];
                    for (int c = f; c > fac; c--) {
                        factores[i] -= ecuaciones[i][c] * factores[c];
                    }
                    fac--;
                }
            	//Se guarda vector Aij del transformador d
                aij.add(factores);
            	//Se resuelve el sistema de ecuaciones con datos de Entrenamiento
                resolverSistemaEntrenamiento(factores, desconocido, posiciones, k);
                //Se resuelve el sistema de ecuaciones con datos de Validación
                resolverSistemaValidacion(factores, desconocido, posiciones, k);
                
                System.out.println("*****");


            }
        }
        
        return resultados;

    }
    
    private void resolverSistemaEntrenamiento(double[] factores, int desconocido, int[] posiciones, int k){
            double errorCuad = 0;
            double real;
            double error;
            //
            double[][] mediciones = matrizConsumo_;
            int total = matrizConsumo_.length;
            for (int l = 0; l < training_lines; l++) {
                real = mediciones[l][desconocido];
                error = real;
                for (int i = 0; i < k; i++) {
                    error -= factores[i] * mediciones[l][posiciones[i]];
                }
                // Se calcula la sumatoria de los errores de cada medicion.
                errorCuad += Math.pow(error, 2);
                /*GUARDAR ERROR [desconocido][l]*/
                resultados[l][desconocido] = Math.pow(error, 2);
            }
            // El error cuadratico se guarda al final de las lineas de entrenamiento.
            System.out.println("Error Entrenamiento: " + String.valueOf(errorCuad / training_lines));
            
            resultados[training_lines][desconocido] = errorCuad / training_lines;
        }
    private void resolverSistemaValidacion(double[] factores, int desconocido, int[ 	] posiciones,
            int k){
        double errorCuad = 0;
        // PASO 3: Hallar el error promedio elevado al Cuadrado.
        double real;
        double error;
        //
        double[][] mediciones = matrizConsumo_;
        int total = matrizConsumo_.length;
        /*linea 144 hasta el final*/
        for (int l = training_lines; l < matrizConsumo_.length; l++) {
            real = mediciones[l][desconocido];
            error = real;
            for (int i = 0; i < k; i++) {
                error -= factores[i] * mediciones[l][posiciones[i]];
            }
            // Se calcula la sumatoria de los errores de cada medicion.
            errorCuad += Math.pow(error, 2);
            /*GUARDAR ERROR [desconocido][l]*/
            resultados[l+1][desconocido] = Math.pow(error, 2);

        }
        // El error cuadratico se guarda al final de las lineas de validacion.
        System.out.println("Error Validacion: " + String.valueOf(errorCuad / validation_lines));
        resultados[total+1][desconocido] = errorCuad / validation_lines;
    }
    
    private double[][] readProblem(String fileName, double[][] matriz) throws FileNotFoundException, IOException {
        double min = 999999999999.00;
        double max = -999999999999.00;
        // Se pasa por parametro el archivo a ser leido.
        BufferedReader bufferedReader = new BufferedReader(new FileReader(fileName));
        // En la primera linea se obtiene la cantidad de transformadores.
        int n = Integer.parseInt(bufferedReader.readLine());
        // En la segunda linea se obtiene la cantidad de muestras.
        int T = Integer.parseInt(bufferedReader.readLine());
        numberOfMuestras_ = T;
        // Se crea la matriz de medicions que mide n x T.
        matriz = new double[numberOfMuestras_][numberOfTrafos_];
        // Se leen las mediciones de cada transformador en el archivo.
        for (int i = 0; i < T; i++) {
            String[] fila = bufferedReader.readLine().split(";");
            for (int j = 0; j < n; j++) {
                matriz[i][j] = Double.parseDouble(fila[j]);
                if (matriz[i][j] < min) {
                    min = matriz[i][j];
                }
                if (matriz[i][j] > max) {
                    max = matriz[i][j];
                }
            }
        }
        bufferedReader.close();
        return matriz;
    }

    public int[] getConfiguracion(String conf, int n) {
        String array[] = conf.split(" ");
        int i = -1;
        nodos = new int[n];
        for (String s : array) {
            nodos[++i] = Integer.parseInt(s);
        }
        return nodos;
    }
    
    
    public void guardar_min_max_desv(){
    	FileWriter fwf;
		try {
			fwf = new FileWriter( DATA_FOLDER + this.configuracion.replace(" ", "") + "-Factores.csv" );
			PrintWriter pwf = new PrintWriter(fwf);
			double [][] datos = new double[n-k][3];
			int i = 0;
	        for (double[] factores: this.aij){
	        	double min = factores[0];
	        	double max = 0.0;
	        	double suma = 0.0;
	        	for (int trans = 0; trans < factores.length; trans++){
	        		suma += factores[trans];
	        		min = (factores[trans] > min) ? min : factores[trans];
	        		max = (factores[trans] < max) ? max : factores[trans];
	            }
	        	double media = suma/factores.length;
	        	double desv = 0.0;
	        	for (int trans = 0; trans < this.k; trans++){
	        		desv += Math.pow((media-factores[trans]), 2);
	            }
	        	desv/=factores.length;
	        	desv = Math.sqrt(desv);
	        	datos[i][0] = min;
	        	datos[i][1] = max;
	        	datos[i][2] = desv;
	        	i++;
	        }
	        for (int trans = 0; trans < 3; trans++){
	        	for (int j = 0; j < n-k; j++) {
	        		if (j==0 && trans==0){pwf.print("Minimo");pwf.print(";");}
	        		if (j==0 && trans==1){pwf.print("Maximo");pwf.print(";");}
	        		if (j==0 && trans==2){pwf.print("Desviacion");pwf.print(";");}
                    pwf.print(String.valueOf(datos[j][trans]).replace('.', ','));pwf.print(";");
                }
	        	pwf.println("");
            }
	        
	        pwf.flush(); pwf.close(); fwf.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
        
    }
    public void guardar_errores(Double[][] resultados){
	 try {
		FileWriter fw = new FileWriter( DATA_FOLDER + configuracion.replace(" ", "") + ".csv" );
		PrintWriter pw = new PrintWriter(fw);
		
		pw.print(";");
		for (int trans = 0; trans < n; trans++){
			if (nodos[trans] == 0){
				pw.print(String.valueOf(trans) + ";");
			}
		}
		pw.println("");
		for (int muestra =  0; muestra < numberOfMuestras_+2; muestra++) {	
		    for (int trans = 0; trans < n; trans++){
		    	if (trans==0){
	    			if (muestra==training_lines)
	    			{
	    				pw.print("Error entrenamiento: ;");
	    			}else if (muestra==numberOfMuestras_+1)
	    			{
	    				pw.print("Error validación: ;");
	    			}
	    			else
	    			{
	    				pw.print("");pw.print(";");
	    			}
		    	}
		    	if (nodos[trans] == 0){
		            pw.print(String.valueOf(resultados[muestra][trans]).replace('.', ','));
		            pw.print(";");
		        }
		    }
		    pw.println("");
		}
		pw.flush(); pw.close();fw.close();
	} catch (IOException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
	}
    }
    public static void main(String[] args) {
        try {
            int n;
            n = 40;
            SCICTD programa = new SCICTD(40, 2);
            programa.configuracion =   "1 0 1 1 1 1 1 0 1 1 " 
        							 + "1 1 1 1 1 0 1 1 1 1 " 
    							 	 + "1 1 1 0 1 1 1 1 1 1 " 
							 		 + "1 1 1 1 1 1 1 1 1 0 ";
                           
            System.out.println("Ejecutando programa. Configuracion: " + programa.configuracion);
            Double [][] resultados = programa.evaluate(programa.getConfiguracion(programa.configuracion, n));
            
            programa.guardar_errores(resultados);
            programa.guardar_min_max_desv();
            

        } catch (FileNotFoundException ex) {
            Logger.getLogger(SCICTD.class.getName()).log(Level.SEVERE, null, ex);
            System.out.println("Error: No se encontro el archivo.");
        } catch (IOException ex) {
            Logger.getLogger(SCICTD.class.getName()).log(Level.SEVERE, null, ex);
            System.out.println(ex.getMessage());
        } catch (ClassNotFoundException ex) {
            Logger.getLogger(SCICTD.class.getName()).log(Level.SEVERE, null, ex);
            System.out.println(ex.getMessage());
        }

    }
}
