#include <mpi.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {
    int rank, size;
    char mensaje[100];
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        printf("Se necesitan al menos 2 procesos\n");
        MPI_Finalize();
        return 0;
    }

    if (rank == 0) {
        strcpy(mensaje, "Hola desde proceso 0");
        MPI_Send(mensaje, strlen(mensaje) + 1, MPI_CHAR,
                 1, 0, MPI_COMM_WORLD);
        MPI_Recv(mensaje, 100, MPI_CHAR, 1, 0,
                 MPI_COMM_WORLD, &status);
        printf("Proceso 0 recibio: %s\n", mensaje);
    } else if (rank == 1) {
        MPI_Recv(mensaje, 100, MPI_CHAR, 0, 0,
                 MPI_COMM_WORLD, &status);
        printf("Proceso 1 recibio: %s\n", mensaje);
        strcpy(mensaje, "Hola desde proceso 1");
        MPI_Send(mensaje, strlen(mensaje) + 1, MPI_CHAR,
                 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
