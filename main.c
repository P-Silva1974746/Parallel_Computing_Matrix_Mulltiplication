#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
#include <math.h>

void fox_min_plus(int N, int block_size, int Q, int **A, int **C, MPI_Comm comm);

int main(int argc, char *argv[]) {
    int N;
    int P;
    int Q;
    int rank;
    int **Di; //matriz inicial
    int **Df; //matriz final

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &P);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    Q = (int) sqrt(P);

    if (rank == 0) {  
        Di = read_input(&N, P, Q); //lê a matriz e armazena em Di; a funçao read_input está em matrix.c
    }

    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD); //faz broadcast de N para todos os processos
    int block_size = N / Q;

    int **A_submatrix = allocate_matrix(block_size);

    //proximos passos em main:
    //distribuir as submatrizes pelos processos (usando scatter? eu acho), criar um tipo para enviar os dados corretos
    //chamar o fox algorithm
    //obter as matrizes parciais de cada processo (usando gather?) e construir a matriz final

    if (rank == 0) {
        print_matrix(Di, N); //funçao em matrix.c
    }
    
    //libertar o espaço usado nas matrizes e assim

    MPI_Finalize();
    return 0;
}

