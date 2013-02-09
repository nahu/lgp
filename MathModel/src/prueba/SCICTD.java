/**
 * SCICTD.java
 *
 * @author Gustavo Planas, Esteban Benitez
 * @version 1.0
 */

package prueba;

import java.io.*;
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
    public int training_lines = 144;
    public int validation_lines = 104;
    public Double [][] resultados = null;
    
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
        
        System.out.println("Configuracion: " + nodos.toString());
        // Se crea el vector fitness de dos elementos: uno para cada objetivo.
        double[] fitness = new double[6];

        // CALCULO DEL FITNESS (Error promedio)

        // PASO 0: Obtener datos del individuo.
        int k = 0; // Cantidad de medidores.
        for (int i = 0; i < nodos.length; i++) {
            if (nodos[i] == 1) {
                nodos[i] = 1;
                k++;
            }
        }

        int n = numberOfTrafos_; // Cantidad de transformadores.
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
                
                resolverSistemaValidacion(factores, desconocido, posiciones, k);
                /*System.out.println("\nFactores de : " + d);
                for (double dd : factores) {
                    System.out.println(String.valueOf(dd) + ";");
                }*/
                
                // FIN DEL CALCULO de Error promedio.
            }
        }
        // Se promedia el error de todos los transformadores desconocidos.
        fitness[0] = fitness[0] / (n - k);
        fitness[1] = fitness[1] / (n - k);
        fitness[2] = fitness[2] / (n - k);

        // El ultimo objetivo es igual a k / n.
        fitness[5] = (double) k / n;
        // Almacena el Fitness del Individuo.
        System.out.println("Fitness - Objetivo 1: " + fitness[2]);
        System.out.println("Fitness - Objetivo 2: " + fitness[5]);
        return resultados;
        // 0 => Error Cuadratico Promedio
        // 1 => Error Absoluto Promedio
        // 2 => Error Relativo Promedio
        // 3 => Error Absoluto Maximo
        // 4 => Error Relativo Maximo
        // 5 => n/k
    } // evaluate
    
    private void resolverSistemaValidacion(double[] factores, int desconocido, int[] posiciones,
            int k){
        double[] fitness = new double[6];
        double errorCuad = 0;
        double errorAbs = 0;
        double errorRel = 0;
        double errorMax = 0;
        double errRelMax = 0;
        // PASO 3: Hallar el error promedio elevado al Cuadrado.
        double real;
        double error;
        double abs;
        double relativo;
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
            errorCuad += Math.pow(error, 2.0);
            /*GUARDAR ERROR [desconocido][l]*/
            resultados[l][desconocido] = Math.pow(error, 2.0);

            abs = Math.abs(error);
            errorAbs += abs;
            relativo = 100 * abs / real;
            errorRel += relativo;
            if (abs > errorMax) {
                errorMax = abs;
            }
            if (relativo > errRelMax) {
                errRelMax = relativo;
            }
        }
        // Se suman los errores medios del conjunto de muestras de cada transformador desconocido.
        fitness[0] += errorCuad / total-validation_lines;
        fitness[1] += errorAbs / total-validation_lines;
        fitness[2] += errorRel / total-validation_lines;
        if (errorMax > fitness[3]) {
            fitness[3] = errorMax;
        }
        if (errRelMax > fitness[4]) {
            fitness[4] = errRelMax;
        }
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
        /*   if (min < 0 || max > 1)
         {
         double promedio = (max - min) / 2;
         max = max + promedio;
         min = min - promedio;
         for (int i = 0; i < T; i++) 
         {
         for (int j = 0; j < n; j++)
         {
         matriz[i][j] = (matriz[i][j] - min) / (max - min);
         }
         }
         } */
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

    public static void main(String[] args) {
        try {
            int n;
            String configuracion = "";
//            System.out.println("Ingrese n: ");
//            Scanner sc = new Scanner(System.in);
//            n = sc.nextInt();
//            System.out.println("Ingrese la configuracion: ");
//            configuracion = sc.nextLine();
//            while (configuracion.length()!=n){
//                System.out.println("Ingrese la configuracion: ");
//                configuracion = sc.nextLine();
//            }

            n = 40;
            
            configuracion =   "1 0 1 1 1 1 1 0 1 1 " 
				            + "1 1 1 1 1 0 1 1 1 1 " 
				    		+ "1 1 1 0 1 1 1 1 1 1 " 
				            + "1 1 1 1 1 1 1 1 1 0 ";

            SCICTD programa = new SCICTD(40, 2);
                
            FileWriter fw = new FileWriter( DATA_FOLDER + configuracion.replace(" ", "") + ".csv" );
            PrintWriter pw = new PrintWriter(fw);
            Double [][] resultados = programa.evaluate(programa.getConfiguracion(configuracion, n));
            
            
            for (int pos = 0; pos<resultados[0].length; pos++){ //transformador
            	double error = 0;
            	if (programa.nodos[pos] == 0){
		            for (int i=programa.training_lines; i<programa.numberOfMuestras_; i++){ //muestra por trasformador.
		            	error += resultados[i][pos];
		            }
		            resultados[programa.numberOfMuestras_][pos] = error;
		            resultados[programa.numberOfMuestras_+1][pos] = error/programa.validation_lines;
            	}
	            
            }
            
            for (int muestra = programa.training_lines; muestra < programa.numberOfMuestras_+2; muestra++) {
                for (int trans = 0; trans < n; trans++){
                    if (programa.nodos[trans] == 0){
                        pw.print(String.valueOf(resultados[muestra][trans]).replace('.', ','));
                        pw.print(";");
                    }
                }
                pw.println("");
            }
            //Flush the output to the file
            pw.flush();

            //Close the Print Writer
            pw.close();

            //Close the File Writer
            fw.close();

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
