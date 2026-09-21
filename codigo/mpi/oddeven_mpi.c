#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int cmp_int(const void* a, const void* b) {
    int x = *(const int*)a;
    int y = *(const int*)b;
    return (x > y) - (x < y);
}

void merge_low(int* local, const int* recv, int n) {
    int* tmp = malloc(2 * n * sizeof(int));
    int i = 0, j = 0, k = 0;
    while (k < n) {
        if (local[i] <= recv[j])
            tmp[k++] = local[i++];
        else
            tmp[k++] = recv[j++];
    }
    for (k = 0; k < n; k++)
        local[k] = tmp[k];
    free(tmp);
}

void merge_high(int* local, const int* recv, int n) {
    int* tmp = malloc(2 * n * sizeof(int));
    int i = n - 1, j = n - 1, k = n - 1;
    while (k >= 0) {
        if (local[i] >= recv[j])
            tmp[k--] = local[i--];
        else
            tmp[k--] = recv[j--];
    }
    for (k = 0; k < n; k++)
        local[k] = tmp[k];
    free(tmp);
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
    if (N % size != 0) {
        if (rank == 0)
            fprintf(stderr, "Error: N debe ser divisible entre el numero de procesos\n");
        MPI_Finalize();
        return 1;
    }

    int local_n = N / size;
    int* a = NULL;
    int* local_a = malloc(local_n * sizeof(int));
    int* recv_buf = malloc(local_n * sizeof(int));

    if (!local_a || !recv_buf) {
        fprintf(stderr, "Error de memoria en rank %d\n", rank);
        MPI_Finalize();
        return 1;
    }

    if (rank == 0) {
        a = malloc(N * sizeof(int));
        for (int i = 0; i < N; i++)
            a[i] = N - i;
    }

    double t_start = MPI_Wtime();

    MPI_Scatter(a, local_n, MPI_INT, local_a, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    qsort(local_a, local_n, sizeof(int), cmp_int);

    for (int phase = 0; phase < size; phase++) {
        int partner;
        if (phase % 2 == 0)
            partner = (rank % 2 == 0) ? rank + 1 : rank - 1;
        else
            partner = (rank % 2 == 0) ? rank - 1 : rank + 1;

        if (partner < 0 || partner >= size)
            continue;

        MPI_Sendrecv(local_a, local_n, MPI_INT, partner, 0,
                     recv_buf, local_n, MPI_INT, partner, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        if (rank < partner)
            merge_low(local_a, recv_buf, local_n);
        else
            merge_high(local_a, recv_buf, local_n);
    }

    int* sorted = NULL;
    if (rank == 0)
        sorted = malloc(N * sizeof(int));

    MPI_Gather(local_a, local_n, MPI_INT, sorted, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    double t_end = MPI_Wtime();

    if (rank == 0) {
        int ok = 1;
        for (int i = 1; i < N; i++)
            if (sorted[i - 1] > sorted[i]) {
                ok = 0;
                break;
            }

        printf("MPI,%d,%d,%e,%d\n",
               N, size, t_end - t_start, ok);

        if (N <= 20) {
            for (int i = 0; i < N; i++)
                printf("%d ", sorted[i]);
            printf("\n");
        }

        free(a);
        free(sorted);
    }

    free(local_a);
    free(recv_buf);

    MPI_Finalize();
    return 0;
}
