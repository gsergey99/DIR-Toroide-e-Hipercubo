
all: 
	
	mpicc Toroide.c -o Toroide

run-toroide:

	mpirun -n 16 ./Toroide
	