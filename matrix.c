#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
#include <string.h>


int** read_input(int *N, int P, int Q) { //funçao para ler a matriz do input
    
    if (scanf("%d", N) != 1) {
        fprintf(stderr, "Input inválido (não foi possível ler N).\n"); //quando nao se consegue ler N for some reason, ns se é relevante ter isto ou se é sempre valido
        exit(EXIT_FAILURE);
    }

    //verifica se o grafo tem um numero valido de nós antes de alocar a memoria
    if (Q * Q != P || (*N) % Q != 0) {
        fprintf(stderr, "Erro: dimensão da matriz (%d) e número de processos (%d) incompatíveis com o algoritmo Fox.\n", *N, P);
        exit(EXIT_FAILURE);
    }

    int **Di = allocate_matrix(*N);

    //lê a matriz do input e armazena em Di
    for (int i = 0; i < *N; i++) {
        for (int j = 0; j < *N; j++) {
            if (scanf("%d", &Di[i][j]) != 1) {
                fprintf(stderr, "Input inválido na linha %d, coluna %d.\n", i, j); //tbm nao sei se isto é relevante ou se vai ser sempre valido
                free_matrix(Di);
                exit(EXIT_FAILURE);
            }
        }
    }
    return Di;
}

int **allocate_matrix(int n) {
    int **m = malloc(n * sizeof(int *)); //cria um array de ponteiros de tamanho n
    int *data = malloc(n * n * sizeof(int)); //cria um array com todos os valores da matriz de forma continua
    for (int i = 0; i < n; i++)
        m[i] = &data[i * n]; //cada valor de m é um ponteiro para o inicio de cada linha
    return m;
}

void free_matrix(int **m) {
    free(m[0]);
    free(m);
}

void copy_matrix(int n, int **src, int **dest) { 
    memcpy(dest[0], src[0], n * n * sizeof(int)); //supostamente é mais eficiente assim mas esta é outra opção:
}

/*void copy_matrix(int n, int **src, int **dest) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            dest[i][j] = src[i][j];
}*/

//min-plus matrix multiplication - altera os valores de C de acordo com os valores de A e B
void min_plus_multiply(int n, int **A, int **B, int **C) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                int val = A[i][k] + B[k][j];
                if (val < C[i][j]) C[i][j] = val;
            }
        }
    }
}

void print_matrix(int **M, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", M[i][j]);
        }
        printf("\n");
    }
}