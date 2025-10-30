#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
#include <math.h>

void repeated_squaring (int N, int block_size, int Q, int **A, int **C, MPI_Comm comm);

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
        Di = read_input(&N, P, Q); //lê a matriz e armazena em Di
        //print_matrix(Di, N);
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (Di[i][j] == 0 && i != j) {
                    Di[i][j] = INF;
                }
            }
        }
        Df = allocate_matrix(N);
    }

    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD); //faz broadcast de N para todos os processos

    int block_size = N / Q;
    
    MPI_Datatype blocktype, resized_block;
    MPI_Type_vector(block_size,       //numero de linhas a enviar (blocos)
                    block_size,       //numero de elementos por linha
                    N,                //stride - numero de elementos entre inícios de linhas consecutivas
                    MPI_INT,
                    &blocktype);

    MPI_Type_create_resized(blocktype,                 //tipo antigo
                            0,                         //endereço inicial
                            block_size * sizeof(int),  //distância entre elementos consecutivos -- alternativa: só sizeof(int); mudar os displs
                            &resized_block);
    MPI_Type_commit(&resized_block);
    MPI_Type_free(&blocktype);

    //definir os displacements e counts para enviar por scatterv
    int *displs, *counts;
    if (rank == 0) {
        displs = malloc(P * sizeof(int));
        counts = malloc(P * sizeof(int));
        for (int i = 0; i < Q; ++i) {
            for (int j = 0; j < Q; ++j) {
                int dest = i * Q + j;
                counts[dest] = 1; 
                displs[dest] = block_size * i * Q + j; //ou i*block_size*N + j*block_size
            }
        }
    }

    int **A_submatrix = allocate_matrix(block_size);

    MPI_Scatterv((rank == 0) ? Di[0] : NULL,  //variavel que contém os elementos a serem enviados (Di)
                counts,                       //numero de elementos a enviar a cada processo (1 para todos)
                displs,                       //deslocamentos para cada processo
                resized_block,                //tipo a ser enviado
                A_submatrix[0],               //variavel onde os dados vao ser armazenados
                block_size * block_size,      //quantidade de elementos que cada processo vai receber
                MPI_INT,                      //tipo a ser recebido
                0,
                MPI_COMM_WORLD);

    int **C = allocate_matrix(block_size);

    //chamar o fox algorithm: 
    repeated_squaring (N, block_size, Q, A_submatrix, C, MPI_COMM_WORLD);

    //obter as matrizes parciais de cada processo usando gather e construir a matriz Df
    // MPI_Gatherv(C[0],                     //variavel com os dados a enviar
    //             block_size * block_size,  //quantidade de elementos a enviar por processo
    //             MPI_INT,                  //tipo a ser enviado
    //             Df ? Df[0] : NULL,        //variavel que receberá todos os dados (Df)
    //             counts,                   //quantidades de elementos a serem recebidos de cada processo
    //             displs,                   //deslocamentos dentro de Df, indica onde guardar os dados de cada processo
    //             resized_block,            //tipo a ser recebido
    //             0,                        //processo root que recebe os dados
    //             MPI_COMM_WORLD);


    int *flat_C = NULL;
    if (rank == 0){
        flat_C = malloc(P * block_size * block_size * sizeof(int));
    }

    MPI_Gather(C[0], block_size * block_size, MPI_INT,
               flat_C, block_size * block_size, MPI_INT,
               0, MPI_COMM_WORLD);



    if (rank == 0) {
    // Rebuild Df from gathered blocks
        for (int i = 0; i < Q; i++) {          // block row
            for (int j = 0; j < Q; j++) {      // block column
                int src_rank = i * Q + j;      // rank that sent this block
                int *src_block = flat_C + src_rank * block_size * block_size;

                for (int bi = 0; bi < block_size; bi++) {
                    for (int bj = 0; bj < block_size; bj++) {
                        Df[i * block_size + bi][j * block_size + bj] =
                            src_block[bi * block_size + bj];
                    }
                }
            }
        }
    }
    if (rank == 0) {
        //printf("MATRIZ FINAL:\n");
        print_matrix(Df, N);
        free_matrix(Di);
        free_matrix(Df);
    }
    
    //libertar o espaço usado nas matrizes e assim
    if(rank==0){
        free(displs);
        free(counts);
    }

    free_matrix(A_submatrix);
    free_matrix(C);
    MPI_Type_free(&resized_block);
    free(flat_C);
    MPI_Finalize();
    return 0;
}



