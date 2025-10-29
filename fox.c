#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
#include <math.h>

void fox (int N, int block_size, int Q, int **A, int **C, MPI_Comm comm) {

    //criar grid_comm e comunicadores row_comm e col_comm (entre linhas e colunas)

    int dims[2] = {Q, Q};
    int periods[2] = {1, 1};
    int reorder = 0;
    MPI_Comm grid_comm, row_comm, col_comm;
    MPI_Cart_create(comm, 2, dims, periods, reorder, &grid_comm);

    int grid_rank, row, col;
    int coords[2];
    MPI_Comm_rank(grid_comm, &grid_rank);
    MPI_Cart_coords(grid_comm, grid_rank, 2, coords);
    row = coords[0];
    col = coords[1];

    int remain_dims[2];

    //row_comm contem os processos na mesma linha
    remain_dims[0] = 0; //linha descartada
    remain_dims[1] = 1; //coluna mantida
    MPI_Cart_sub(grid_comm, remain_dims, &row_comm);

    //col_comm contem os processos na mesma coluna
    remain_dims[0] = 1;
    remain_dims[1] = 0;
    MPI_Cart_sub(grid_comm, remain_dims, &col_comm);

    //alocar espaço para as matrizes a serem usadas nos calculos
    int **A_local = allocate_matrix(block_size);
    int **B_local = allocate_matrix(block_size);
    int **A_temp = allocate_matrix(block_size);

    //copia a submatriz A recebida para A_local e B_local
    copy_matrix(block_size, A, A_local);
    copy_matrix(block_size, A, B_local);

    //inicializa C com INF 
    for (int i = 0; i < block_size; i++)
        for (int j = 0; j < block_size; j++)
            C[i][j] = INF;
            
    for (int iter=0; iter<Q; iter++){
        int root_in_row = (row + iter)%Q; 

        // Sender process
        if(col==root_in_row){
            MPI_Bcast(A_local[0], block_size*block_size, MPI_INT, root_in_row, row_comm);
        }else{
            //receiving processes
            MPI_Bcast(A_temp[0], block_size*block_size, MPI_INT, root_in_row, row_comm);
        }

        // This lines makes it so that the pointer A_used is equal to A_local or A_temp depending on the condition 
        int **A_used = (col == root_in_row) ? A_local : A_temp;

        // Multiplication operation
        min_plus_multiply(block_size, A_used, B_local, C);
        //printf("Matriz C rank %d iter %d:\n", grid_rank, iter);
        //print_matrix(C, block_size);
        
        // B up shift
        int src, dest;
        MPI_Cart_shift(col_comm, 0, -1, &src, &dest);
        MPI_Sendrecv_replace(&B_local[0][0], block_size*block_size, MPI_INT, dest, 0, src, 0, col_comm, MPI_STATUS_IGNORE);
    }

    free_matrix(A_local);
    free_matrix(B_local);
    free_matrix(A_temp);
    MPI_Comm_free(&row_comm);
    MPI_Comm_free(&col_comm);
    MPI_Comm_free(&grid_comm);
}

void repeated_squaring (int N, int block_size, int Q, int **A, int **C, MPI_Comm comm) {

    //esta é a funçao principal que chama o fox até que o numero da iteraçao ultrapasse o numero de nós da matriz inicial ig
    int stage = 1;
    while (stage < N-1) {
        fox(N, block_size, Q, A, C, comm);
        copy_matrix(block_size, C, A); //depois de chamar o fox e obter o resultado em C, coloca esse resultado em A para voltar a chamar fox com o novo resultado
        stage=stage*2;
    }
    
    //substituir infinitos por zeros em C
    for (int i = 0; i < block_size; i++) {
        for (int j = 0; j < block_size; j++) {
            if (C[i][j] == INF)
                C[i][j] = 0;
        }
    }
    
    //no final, a submatriz C está com os valores finais

}
