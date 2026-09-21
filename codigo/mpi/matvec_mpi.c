#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

void init_matrix(double* A, int N) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            A[i * N + j] = (double)(i + j);
}

void init_vector(double* x, int N) {
    for (int j = 0; j < N; j++)
        x[j] = 1.0;
}

void local_matvec(const double* A, const double* x, double* y, int rows, int N) {
    for (int i = 0; i < rows; i++) {
        double sum = 0.0;
        for (int j = 0; j < N; j++)
            sum += A[i * N + j] * x[j];
        y[i] = sum;
    }
}

void matvec_seq(const double* A, const double* x, double* y, int N) {
    for (int i = 0; i < N; i++) {
        double sum = 0.0;
        for (int j = 0; j < N; j++)
            sum += A[i * N + j] * x[j];
        y[i] = sum;
    }
}

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 2) {
        if (rank == 0)
            fprintf(stderr, "Uso: %s <N>\n", argv[0]);
        MPI_Finalize();
        return 1;
    }

    int N = atoi(argv[1]);

    int* rows_per_proc = malloc(size * sizeof(int));
    int* counts = malloc(size * sizeof(int));
    int* displs = malloc(size * sizeof(int));

    int base = N / size;
    int rem = N % size;
    int offset = 0;
    for (int i = 0; i < size; i++) {
        rows_per_proc[i] = base + (i < rem ? 1 : 0);
        counts[i] = rows_per_proc[i] * N;
        displs[i] = offset;
        offset += counts[i];
    }

    int local_rows = rows_per_proc[rank];

    double* A = NULL;
    double* x = malloc(N * sizeof(double));
    double* local_A = malloc(local_rows * N * sizeof(double));
    double* local_y = malloc(local_rows * sizeof(double));
    double* y = NULL;

    if (rank == 0) {
        A = malloc(N * N * sizeof(double));
        y = malloc(N * sizeof(double));
        init_matrix(A, N);
        init_vector(x, N);
    }

    double t_start = MPI_Wtime();

    MPI_Bcast(x, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatterv(A, counts, displs, MPI_DOUBLE,
                 local_A, local_rows * N, MPI_DOUBLE,
                 0, MPI_COMM_WORLD);

    local_matvec(local_A, x, local_y, local_rows, N);

    int* recv_counts = malloc(size * sizeof(int));
    int* recv_displs = malloc(size * sizeof(int));
    offset = 0;
    for (int i = 0; i < size; i++) {
        recv_counts[i] = rows_per_proc[i];
        recv_displs[i] = offset;
        offset += recv_counts[i];
    }

    MPI_Gatherv(local_y, local_rows, MPI_DOUBLE,
                y, recv_counts, recv_displs, MPI_DOUBLE,
                0, MPI_COMM_WORLD);

    double t_end = MPI_Wtime();

    if (rank == 0) {
        double* y_seq = malloc(N * sizeof(double));
        matvec_seq(A, x, y_seq, N);

        double err = 0.0;
        for (int i = 0; i < N; i++) {
            double d = y[i] - y_seq[i];
            err += d * d;
        }
        err = sqrt(err);

        printf("MPI,%d,%d,%e,%e\n",
               N, size, t_end - t_start, err);

        if (N <= 16) {
            printf("Resultado y:\n");
            for (int i = 0; i < N; i++)
                printf("%.2f ", y[i]);
            printf("\n");
        }

        free(y_seq);
    }

    free(A);
    free(x);
    free(local_A);
    free(local_y);
    free(y);
    free(rows_per_proc);
    free(counts);
    free(displs);
    free(recv_counts);
    free(recv_displs);

    MPI_Finalize();
    return 0;
}
