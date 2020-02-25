#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include "mpi.h"

#define archivo "datos.dat"
#define MAX_BUFFER 1000
 


int leer_fichero();
void conocer_vecinos(int rank);


int main(int argc, char *argv[]){

    
    
    int rank, size, i,L;
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    L = sqrt(size);
    int num_nodos = L*L;
    float numeros[L*L];
    int num_leidos;
       

    if(rank ==0){
        if(size!=num_nodos){
            fprintf(stderr,"Error, el número de nodos (%d) no coincide con las dimensiones del Toroide (%d)\n",num_nodos,size);
            continuar =0;
        }else{

                num_leidos = leer_fichero();
            }            
        
        }

    }

    MPI_Finalize();
    return 0;
}

int leer_fichero(){
    FILE* fichero;
    char *buffer = malloc(MAX_BUFFER*sizeof(char));
    double lista_numeros[56];
    int num_numeros=0;
    char *token;
    fichero = fopen(archivo,"r");

    if (fichero == NULL){
        fprintf(stderr,"Error en la lectura del archivo %s\n",archivo);
        exit(EXIT_FAILURE);
    }
    while (fscanf(fichero,"%s",buffer)!=EOF)
    {
        printf("%s\n",buffer);
    }
    fclose(fichero);

    token = strtok(buffer,",");
    while(token != NULL) {
      lista_numeros[num_numeros] = atof(token);
      printf("%f \n",lista_numeros[num_numeros]);
      token = strtok(NULL,",");
      num_numeros++;
    }
    return num_numeros;

}
void conocer_vecinos(int rank){
    int norte,sur,este,oeste;
    int fila = rank/L;
    int columna = rank%L;

    switch (fila) // Saber los vecinos NORTE y SUR
    {
    case 0:
        norte = rank+L;
        sur = (L*(L-1))+rank;
        break;
    
    case L-1:
        norte = columna;
        sur = rank-L;
        break;
    
    default:
        norte = rank+L;
        sur = rank-L;
        break;
    }

    switch (columna) //Saber los vecinos ESTE y OESTE
    {
    case 0:
        este = rank+1;
        oeste = rank+(L-1);
        break;
    
    case L-1:
        este = rank-(L-1);
        oeste = rank-1;
        break;
    default:
        este = rank+1;
        oeste = rank-1;
        break;
    }

    printf("Los vecinos de %d son NORTE: %d, SUR: %d, ESTE: %d, OESTE: %d\n",rank,norte,sur,este,oeste);

}