#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../common/timer.h"

void init_matrix(double* A, int N) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            A[i * N + j] = (double)(i + j);
}

void init_vector(double* x, int N) {
    for (int j = 0; j < N; j++)
        x[j] = 1.0;
}

void matvec(const double* A, const double* x, double* y, int N) {
    for (int i = 0; i < N; i++) {
        double sum = 0.0;
        for (int j = 0; j < N; j++)
            sum += A[i * N + j] * x[j];
        y[i] = sum;
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <N>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);
    double* A = malloc(N * N * sizeof(double));
    double* x = malloc(N * sizeof(double));
    double* y = malloc(N * sizeof(double));

    if (!A || !x || !y) {
        fprintf(stderr, "Error: no se pudo reservar memoria\n");
        return 1;
    }

    init_matrix(A, N);
    init_vector(x, N);

    double t_start = get_time();
    matvec(A, x, y, N);
    double t_end = get_time();

    printf("SECUENCIAL,%d,%e\n", N, t_end - t_start);

    if (N <= 16) {
        printf("Resultado y:\n");
        for (int i = 0; i < N; i++)
            printf("%.2f ", y[i]);
        printf("\n");
    }

    free(A);
    free(x);
    free(y);
    return 0;
}
