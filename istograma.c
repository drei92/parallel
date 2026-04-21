#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// Función para determinar el índice del bin de forma constante O(1)
int get_bin(float val, float min_meas, int bin_count, float bin_width) {
    if (val < min_meas) return 0;
    int bin = (int)((val - min_meas) / bin_width);
    if (bin >= bin_count) bin = bin_count - 1;
    return bin;
}

int main(int argc, char** argv) {
    int my_rank, comm_sz;
    int data_count, bin_count;
    float min_meas, max_meas, bin_width;
    float *data = NULL, *local_data = NULL;
    int *bin_counts = NULL, *local_bin_counts = NULL;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    // 1. Entrada de datos en el Proceso 0
    if (my_rank == 0) {
        printf("Configuración del Histograma\n");
        printf("Cantidad de datos: "); scanf("%d", &data_count);
        printf("Mínimo y Máximo: "); scanf("%f %f", &min_meas, &max_meas);
        printf("Cantidad de Bins: "); scanf("%d", &bin_count);
        
        data = malloc(data_count * sizeof(float));
        printf("Ingrese los datos uno por uno:\n");
        for (int i = 0; i < data_count; i++) scanf("%f", &data[i]);
    }

    // 2. Transmisión de parámetros a todos los procesos
    MPI_Bcast(&data_count, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&min_meas, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&max_meas, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&bin_count, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Cálculos derivados
    bin_width = (max_meas - min_meas) / bin_count;
    int local_data_count = data_count / comm_sz;

    // 3. Reparto de los datos
    local_data = malloc(local_data_count * sizeof(float));
    MPI_Scatter(data, local_data_count, MPI_FLOAT, 
                local_data, local_data_count, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // 4. Conteo local
    local_bin_counts = calloc(bin_count, sizeof(int));
    for (int i = 0; i < local_data_count; i++) {
        int b = get_bin(local_data[i], min_meas, bin_count, bin_width);
        local_bin_counts[b]++;
    }

    // 5. Reducción global
    if (my_rank == 0) bin_counts = malloc(bin_count * sizeof(int));
    
    MPI_Reduce(local_bin_counts, bin_counts, bin_count, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // 6. Impresión de resultados
    if (my_rank == 0) {
        printf("\n--- RESULTADOS DEL HISTOGRAMA ---\n");
        for (int i = 0; i < bin_count; i++) {
            printf("Bin %d [%.2f, %.2f]: %d\n", i, 
                   min_meas + i*bin_width, min_meas + (i+1)*bin_width, bin_counts[i]);
        }
        free(data);
        free(bin_counts);
    }

    free(local_data);
    free(local_bin_counts);
    MPI_Finalize();
    return 0;
}