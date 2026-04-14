#include <stdio.h>
#include <string.h>
#include <mpi.h>

const int MAX_STRING = 100;

int main(void) {
    int comm_sz, my_rank;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (my_rank != 0) {
        char greeting[MAX_STRING];
        sprintf(greeting, "Saludos del proceso %d!", my_rank);
                
        // OPCIÓN 1: Correcto, envía el fin de cadena '\0'
        // MPI_Send(greeting, strlen(greeting) + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);

        // OPCIÓN 2: Envia basura
        // MPI_Send(greeting, strlen(greeting), MPI_CHAR, 0, 0, MPI_COMM_WORLD);

        // OPCIÓN 3: Envía bytes innecesarios
        MPI_Send(greeting, MAX_STRING, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        // =============================================================

    } else {
        printf("Proceso 0 (Receptor) reportando:\n");
        for (int q = 1; q < comm_sz; q++) {
            char message[MAX_STRING];
            
            // Llenamos el buffer de '#' para que la basura sea visible
            memset(message, '#', MAX_STRING); 
            
            MPI_Recv(message, MAX_STRING, MPI_CHAR, q, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            // Al imprimir con %s, printf busca el primer '\0'
            printf("  Mensaje de %d: %s\n", q, message);
        }
    }

    MPI_Finalize();
    return 0;
}