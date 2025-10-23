#ifndef MATRIX_H
#define MATRIX_H

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define INF 10000000

int** read_input(int *N, int P, int Q);
int **allocate_matrix(int n);
void free_matrix(int **m);
void copy_matrix(int n, int **src, int **dest);
void min_plus_multiply(int n, int **A, int **B, int **C);
void print_matrix(int **M, int N);

#endif
