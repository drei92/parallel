#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    int rank, size;
    char nombre[MPI_MAX_PROCESSOR_NAME];
    int len;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_processor_name(nombre, &len);

    printf("Proceso %d de %d ejecutandose en %s\n", rank, size, nombre);

    MPI_Finalize();
    return 0;
}
