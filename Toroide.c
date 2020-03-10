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
#define L 4



int leer_fichero(float numeros[]);
void conocer_vecinos(int rank, int vecinos[]);


int main(int argc, char *argv[]){

    int rank, size; //Definimos las variables rank y size
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int num_leidos,opcion;
    float numeros[L*L];
    int provisional[L*L] ={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}; 
    float buffer, min_rank;
    int vecinos[4]; // 0->Norte,1->Sur,2->Este,3->Oeste
    MPI_Status status;


    if(rank ==0){
        if(size!=L*L){
            opcion = 0; // El rank 0 va finalizar todos los nodos mandando un mensaje a todos los que en el comunicador, MultiCast
            fprintf(stderr,"Error, el número de nodos (%d) no coincide con las dimensiones del Toroide (%d)\n",size,L*L);
            MPI_Bcast(&opcion, 1, MPI_INT,0, MPI_COMM_WORLD);

        }else{

            num_leidos = leer_fichero(numeros); //leemos los numeros del archivo datos.dat

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

        MPI_Recv(&buffer,1,MPI_FLOAT,0,0,MPI_COMM_WORLD,&status);
        conocer_vecinos(rank,vecinos);
        min_rank = buffer; // Guardamos el valor recibido del rank 0, en el valor mínimo de cada nodo
        for (int j=0;j<L;j++){

            

            MPI_Send(&min_rank,1,MPI_FLOAT,vecinos[0],10,MPI_COMM_WORLD); //Mandamos a los nodos del NORTE el valor del buffer

            MPI_Recv(&buffer,1,MPI_FLOAT,vecinos[1],10,MPI_COMM_WORLD,&status); // Recibimos de los nodos del SUR el valor del buffer que ellos hayan mandado
            
            if(buffer<min_rank){ //Comparamos el valor recibido del SUR (buffer) con el valor mínimo del nodo
                min_rank = buffer;
            }
        }

        for (int j=0;j<L;j++){

        
            MPI_Send(&min_rank,1,MPI_FLOAT,vecinos[2],32,MPI_COMM_WORLD); //Mandamos a los nodos del ESTE el valor del buffer

            MPI_Recv(&buffer,1,MPI_FLOAT,vecinos[3],32,MPI_COMM_WORLD,&status); // Recibimos de los nodos del OESTE el valor del buffer que ellos hayan mandado

            if(buffer<min_rank){ //Comparamos el valor recibido del OESTE (buffer) con el valor mínimo del nodo
                min_rank = buffer;
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
    int fila = rank/L;
    int columna = rank%L;


    switch (fila) // Saber los vecinos NORTE->0 y SUR->1
    {
    case 0:
        vecinos[0] = rank+L;
        vecinos[1] = (L*(L-1))+rank;
        break;

    case L-1:
        vecinos[0] = columna;
        vecinos[1] = rank-L;
        break;

    default:
        vecinos[0] = rank+L;
        vecinos[1] = rank-L;
        break;
    }

    switch (columna) //Saber los vecinos ESTE -> 2 y OESTE -> 3
    {
    case 0:
        vecinos[2] = rank+1;
        vecinos[3] = rank+(L-1);
        break;

    case L-1:
        vecinos[2] = rank-(L-1);
        vecinos[3] = rank-1;
        break;
    default:
        vecinos[2] = rank+1;
        vecinos[3] = rank-1;
        break;
    }

} 