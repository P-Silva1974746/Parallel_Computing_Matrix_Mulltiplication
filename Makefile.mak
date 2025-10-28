#im not sure como fazer um makefile ainda mas eu compilo assim

mpicc -o fox main.c fox.c matrix.c -lm
mpirun -np 4 ./fox < inputx 


