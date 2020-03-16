#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include "mpi.h"

#define archivo "datos.dat"
#define MAX_BUFFER 1000
#define D 4



int leer_fichero(float numeros[]);
void conocer_vecinos(int rank, int vecinos[]);
float maximo(int rank,int vecinos[],float buffer_nodo);

int main(int argc, char *argv[]){

    int rank, size; //Definimos las variables rank y size
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int num_leidos,opcion,dimension;
    dimension = pow(2,D); // Numero de nodos del hipercubo
    float numeros[dimension]; //Definimos el array de números reales que contendrá el fichero 
    float buffer, max_rank;
    int vecinos[D]; //Array de vecinos de cada uno de los nodos
    MPI_Status status;
    MPI_Request request;


    if(rank ==0){
        if(size!=dimension){
            opcion = 0; // El rank 0 va finalizar todos los nodos mandando un mensaje a todos los que en el comunicador, MultiCast
            fprintf(stderr,"Error, el número de nodos (%d) no coincide con las dimensiones del Toroide (%d)\n",size,dimension);
            MPI_Bcast(&opcion, 1, MPI_INT,0, MPI_COMM_WORLD);

        }else{

            num_leidos = leer_fichero(numeros); //leemos los numeros del archivo datos.dat
            if(size!=num_leidos){ //El rank 0 va finalizar todos los nodos mandando un mensaje a todos los que en el comunicador, MultiCast
                opcion=0; 
                fprintf(stderr,"Error, el número de nodos (%d) no coincide con la cantidad de números leídos (%d)\n",size,num_leidos);
                MPI_Bcast(&opcion,1, MPI_INT,0, MPI_COMM_WORLD);

            }else{

                for (int i=0;i<num_leidos;i++){
                    buffer = numeros[i];
                    MPI_Send(&buffer,1,MPI_FLOAT,i,0,MPI_COMM_WORLD); // El rank 0 envía a todos los nodos, 
                    //incluyéndose así mismo él mismo, los valores del fichero
                        

                }
                opcion = 1; // El rank 0 evía a todos los procesos un mensaje de continuación, a través de un mensaje MultiCast del comunicador
                MPI_Bcast(&opcion,1,MPI_INT,0, MPI_COMM_WORLD);
            }
        }            
    }
    //Si la opción es 1,continuamos con la ejecución. Si es 0, se anula la ejecución de los nodos
    MPI_Bcast(&opcion, 1, MPI_INT, 0, MPI_COMM_WORLD); // Cada nodo recibe el aviso del rank 0, 
    //para indicar de que se ha realizado correctamente el envío de datos al fichero

    if(opcion==1){

        MPI_Recv(&buffer,1,MPI_FLOAT,0,0,MPI_COMM_WORLD,&status); //El nodo recibe el valor enviado por el nodo 0
        conocer_vecinos(rank,vecinos); //Obtenemos los vecinos de cada nodo
        max_rank = maximo(rank,vecinos,buffer); // Obtenemos el máximo de toda la red
        
        if (rank==0){

            printf("[RANK %d] El valor minimo es %3f\n",rank,max_rank); // El rank 0 imprimirá el valor máximo de toda la red
        }
        
        
        
    }
    MPI_Finalize();
    return 0;
}

int leer_fichero(float lista_numeros[]){
    FILE* fichero;
    char *buffer = malloc(MAX_BUFFER*sizeof(char));
    int num_numeros=0;
    char *token;
    fichero = fopen(archivo,"r");

    if (fichero == NULL){
        fprintf(stderr,"Error en la lectura del archivo %s\n",archivo);
        exit(EXIT_FAILURE);
    }
    while (fscanf(fichero,"%s",buffer)!=EOF)
    {

    }
    fclose(fichero);

    token = strtok(buffer,",");
    while(token != NULL) {
      
      lista_numeros[num_numeros] = atof(token);
      token = strtok(NULL,",");
      num_numeros++;
    }
    return num_numeros;

}

void conocer_vecinos(int rank,int vecinos[]){

    for(int i=0;i<D;i++){
        vecinos[i] = rank ^ (1 << i);  //Con el desplazamiento obtenemos el el vecino correspondiente de cada nodo
    
    }
}

float maximo (int rank, int vecinos[],float buffer_nodo){
    float max_rank;
    MPI_Status status;
    MPI_Request request;
    max_rank = buffer_nodo; // Guardamos el valor recibido del rank 0, en el valor máximo de cada nodo
    for (int i=0;i<D;i++){ // Ralizamos un for para poder mandar el valor máximo a cda uno de los vecinos

        MPI_Isend(&max_rank,1,MPI_FLOAT,vecinos[i],D,MPI_COMM_WORLD,&request); //Mandamos a los nodos vecinos el valor del buffer

        MPI_Recv(&buffer_nodo,1,MPI_FLOAT,vecinos[i],D,MPI_COMM_WORLD,&status); // Recibimos de los vecinos el valor del buffer que ellos hayan mandado
                
        MPI_Wait(&request,&status);
        if(buffer_nodo>max_rank){ //Comparamos el valor recibido del vecino con el del buffer del nodo
            max_rank = buffer_nodo;
        }
            
    }
        return max_rank; 

}