#include <stdio.h>
#include <mpi.h>

int main(void) {
    int my_rank, comm_sz;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    if (my_rank == 0) {
        // El proceso 0 empieza de inmediato
        printf("proceso %d de %d\n", my_rank, comm_sz);
        
        // envia la senal al siguiente proceso
        if (comm_sz > 1) {
            int token = 0;
            MPI_Send(&token, 0, MPI_INT, 1, 0, MPI_COMM_WORLD);
        }
    } else {
        // esperan la senal del cluster anterior
        int token;
        MPI_Recv(&token, 0, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        printf("proceso %d de %d\n", my_rank, comm_sz);
        
        // envia la senal al siguiente proceso, ultimo = no envia senal
        if (my_rank < comm_sz - 1) {
            MPI_Send(&token, 0, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;
}