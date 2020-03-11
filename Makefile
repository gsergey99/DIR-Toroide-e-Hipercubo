
all: 
	
	mpicc Toroide.c -o Toroide
	mpicc Hipercubo.c -o Hipercubo

run-toroide:

	mpirun -n 16 ./Toroide
	
run-hipercubo:

	mpirun -n 16 ./Hipercubo