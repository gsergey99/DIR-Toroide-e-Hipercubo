#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
//#include "mpi.h"

#define archivo "datos.dat"
#define MAX_BUFFER 1000
#define L 4 

void leer_fichero();

int main(int argc, char *argv[]){

    int rank, size;
    int continuar = 1;
    int num_nodos = L*L;
    
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    if(rank ==0){
        if(size!=num_nodos){
            fprintf(stderr,"Error, el número de nodos (%d) no coincide con las dimensiones del Toroide (%d)\n",num_nodos,size);
            continuar =0;
        }

    }

    printf("Hello World\n");
    MPI_Finalize();
    return 0;
}

void leer_fichero(){
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

}