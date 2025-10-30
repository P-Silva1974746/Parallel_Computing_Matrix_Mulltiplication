#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
#include <math.h>


void repeated_squaring (int N, int block_size, int **A, int **C) {

    //esta é a funçao principal que chama o fox até que o numero da iteraçao ultrapasse o numero de nós da matriz inicial ig
    int stage = 1;
    while (stage < N-1) {
        min_plus_multiply(block_size, A, A, C);
        copy_matrix(block_size, C, A); //depois de chamar o  e obter o resultado em C, coloca esse resultado em A para voltar a chamar fox com o novo resultado
        stage=stage*2;
    }
    
    //substituir infinitos por zeros em C

    for (int i = 0; i < block_size; i++) {
        for (int j = 0; j < block_size; j++) {
            if (C[i][j] == INF){
                C[i][j] = 0;
            }
                
        }
    }
    
    //no final, a submatriz C está com os valores finais

}

int main(int argc, char *argv[]) {
    int N;
    int **Di; //matriz inicial

    Di = read_input_seq(&N); //lê a matriz e armazena em Di
    //print_matrix(Di, N);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (Di[i][j] == 0 && i != j) {
                Di[i][j] = INF;
            }
        }
    }
    

    int **C = allocate_matrix(N);
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            C[i][j] = INF;

    //chamar o fox algorithm: 
    repeated_squaring (N, N ,Di , C);


    //  printf("MATRIZ FINAL:\n");
    print_matrix(C, N);


    //libertar o espaço usado nas matrizes e assim
    free_matrix(Di);
    free_matrix(C);


    return 0;
}
