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


int main(int argc, char *argv[]){

    int rank, size; //Definimos las variables rank y size
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int num_leidos,opcion,dimension;
    dimension = pow(2,D);
    float numeros[dimension];
    //float provisional[] ={-10.30,98.36,58.7,-12.10,1.36,100.39,-54.63,87.36};
    //int provisional[dimension] ={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}; 
    float buffer, min_rank;
    int vecinos[D]; 
    MPI_Status status;
    MPI_Request request;


    if(rank ==0){
        if(size!=dimension){
            opcion = 0; // El rank 0 va finalizar todos los nodos mandando un mensaje a todos los que en el comunicador, MultiCast
            fprintf(stderr,"Error, el número de nodos (%d) no coincide con las dimensiones del Toroide (%d)\n",size,dimension);
            MPI_Bcast(&opcion, 1, MPI_INT,0, MPI_COMM_WORLD);

        }else{

            num_leidos = leer_fichero(numeros); //leemos los numeros del archivo datos.dat
            //num_leidos = 8;
            if(size!=num_leidos){ // El rank 0 va finalizar todos los nodos mandando un mensaje a todos los que en el comunicador, MultiCast
                opcion=0; 
                fprintf(stderr,"Error, el número de nodos (%d) no coincide con la cantidad de números leídos (%d)\n",size,num_leidos);
                MPI_Bcast(&opcion,1, MPI_INT,0, MPI_COMM_WORLD);

            }else{

                for (int i=0;i< num_leidos;i++){
                    buffer = numeros[i];
                    MPI_Send(&buffer,1,MPI_FLOAT,i,0,MPI_COMM_WORLD); // El rank 0 envía a todos los nodos, incluyéndose así mismo él mismo, los valores del fichero
                        

                }
                opcion = 1; // El rank 0 va finalizar todos los nodos mandando un mensaje a todos los que en el comunicador, MultiCast
                MPI_Bcast(&opcion,1,MPI_INT,0, MPI_COMM_WORLD);
            }
        }            
    }

    MPI_Bcast(&opcion, 1, MPI_INT, 0, MPI_COMM_WORLD); // Cada nodo recibe el aviso del rank 0, para indicar de que se ha realizado correctamente el envío de datos al fichero

    if(opcion==1){

        MPI_Recv(&buffer,1,MPI_FLOAT,0,0,MPI_COMM_WORLD,&status); //El nodo recibe el valor enviado por el nodo 0
        conocer_vecinos(rank,vecinos);
        min_rank = buffer; // Guardamos el valor recibido del rank 0, en el valor mínimo de cada nodo
        for (int i=0;i<D;i++){ // Realizamos el primer for para cada una de las dimensiones

            for(int j=0;j<D;j++){ //REalizamos un segundo for para cada uno de los vecinos de cada nodo

                MPI_Isend(&min_rank,1,MPI_FLOAT,vecinos[j],10,MPI_COMM_WORLD,&request); //Mandamos a los nodos del NORTE el valor del buffer

                MPI_Recv(&buffer,1,MPI_FLOAT,vecinos[j],10,MPI_COMM_WORLD,&status); // Recibimos de los nodos del SUR el valor del buffer que ellos hayan mandado
                
                MPI_Wait(&request,&status);
                if(buffer<min_rank){ //Comparamos el valor recibido del SUR (buffer) con el valor mínimo del nodo
                    min_rank = buffer;
                }
            }
        }        
        if (rank==0){

            printf("[RANK %d] El valor minimo es %3f\n",rank,buffer); // El rank 0 imprimirá el valor mínimo de toda la red
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
        vecinos[i] = rank ^ (1 << i);  //Con el desplazamiento obtenemos el el vecino correspondiente
    
    }
} 