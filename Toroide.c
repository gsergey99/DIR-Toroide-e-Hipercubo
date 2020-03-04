#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include "mpi.h"

#define archivo "datos.dat"
#define MAX_BUFFER 1000
#define L 4



int leer_fichero(float numeros[]);
void conocer_vecinos(int rank, int vecinos[]);


int main(int argc, char *argv[]){

    int rank, size;
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int num_nodos,num_leidos,min;
    num_nodos = L*L;
    char *opcion;
    float numeros[L*L];
    int provisional[L*L] ={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}; 
    num_leidos = 16;
    int buffer;
    int vecinos[4]; // 0->Norte,1->Sur,2->Este,3->Oeste
    float buffer_1;
    MPI_Status status;
    

    if(rank ==0){
        if(size!=num_nodos){
            opcion = "finalizar";
            fprintf(stderr,"Error, el número de nodos (%d) no coincide con las dimensiones del Toroide (%d)\n",size,num_nodos);
            MPI_Bcast(&opcion, 1, MPI_CHAR,0, MPI_COMM_WORLD);
            
        }else{
            
            //num_leidos = leer_fichero(numeros); //leemos los numeros del archivo datos.dat

            if(size!=num_leidos){
                opcion="finalizar"; 
                fprintf(stderr,"Error, el número de nodos (%d) no coincide con la cantidad de números leídos (%d)\n",size,num_leidos);
                MPI_Bcast(&opcion,1, MPI_CHAR,0, MPI_COMM_WORLD);

            }else{
                
                opcion = "continuar";
                MPI_Bcast(&opcion, 1,MPI_CHAR,0, MPI_COMM_WORLD);
            
                for (int i=0;i< num_leidos;i++){
                    buffer = provisional[i];
                    MPI_Send(&buffer,1,MPI_INT,i,0,MPI_COMM_WORLD);    
                    

                }
            }
        }            
    }

    MPI_Bcast(&opcion, 1, MPI_CHAR, 0, MPI_COMM_WORLD);

    if(strcmp(opcion,"continuar")){

        MPI_Recv(&buffer,1,MPI_INT,0,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
        conocer_vecinos(rank,vecinos);
        
        for (int j=0;j<L;j++){
            
            MPI_Recv(&min,1,MPI_INT,vecinos[1],MPI_ANY_TAG,MPI_COMM_WORLD,&status);

            if(buffer<min){
                buffer = min;
            }

            MPI_Send(&buffer,1,MPI_INT,vecinos[0],0,MPI_COMM_WORLD);    
        }
        for (int j=0;j<L;j++){
            
            MPI_Recv(&min,1,MPI_INT,vecinos[3],MPI_ANY_TAG,MPI_COMM_WORLD,&status);

            if(buffer<min){
                buffer = min;
            }
            MPI_Send(&buffer,1,MPI_INT,vecinos[2],0,MPI_COMM_WORLD);    

        

        }
        if (rank ==0){
            printf("El valor minimo es %d\n",buffer);
        }


    MPI_Finalize();
    return 0;
    }
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
      //printf("%f \n",lista_numeros[num_numeros]);
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