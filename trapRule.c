#include <stdio.h>
#include <mpi.h>

// La función que vamos a integrar: f(x) = x^2
double f(double x) {
    return x * x;
}

// Función Trap (Program 3.3 del libro)
double Trap(double left_endpt, double right_endpt, int trap_count, double base_len) {
    double estimate, x;
    int i;

    estimate = (f(left_endpt) + f(right_endpt)) / 2.0;
    for (i = 1; i <= trap_count - 1; i++) {
        x = left_endpt + i * base_len;
        estimate += f(x);
    }
    estimate = estimate * base_len;
    return estimate;
}

int main(void) {
    int my_rank, comm_sz, n = 100, local_n; // n=10 para probar el residuo
    double a = 0.0, b = 3.0, h, local_a, local_b;
    double local_int, total_int;
    int source;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    h = (b - a) / n; 

    /* aqui los residuos */
    int remainder = n % comm_sz;
    if (my_rank < remainder) {
        local_n = n / comm_sz + 1;
        local_a = a + my_rank * local_n * h;
    } else {
        local_n = n / comm_sz;
        local_a = a + (my_rank * local_n + remainder) * h;
    }
    local_b = local_a + local_n * h;

    local_int = Trap(local_a, local_b, local_n, h);

    if (my_rank != 0) {
        MPI_Send(&local_int, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    } else {
        total_int = local_int;
        for (source = 1; source < comm_sz; source++) {
            MPI_Recv(&local_int, 1, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total_int += local_int;
        }
    }

    if (my_rank == 0) {
        printf("Con n = %d trapecios, nuestra estimacion\n", n);
        printf("de la integral de %f a %f = %.15e\n", a, b, total_int);
    }

    MPI_Finalize();
    return 0;
}


/*
Variables Locales
my_rank: Es la identidad del proceso. Fundamental para que el proceso sepa qué parte de la integral le toca.

local_a y local_b: Son los límites del sub-intervalo. Por ejemplo, el proceso 0 calcula de 0 a 1, y el proceso 1 de 1 a 2.

local_n: El número de trapecios que calculará ese proceso específico (especialmente importante con la modificación de balanceo de carga que hicimos).

local_int: El resultado parcial de la integral calculado por un solo proceso.

Variables Globales
comm_sz: Todos los procesos deben saber cuántos procesos totales hay en el comunicador.

n: El número total de trapecios en los que se divide la integral completa.

a y b: Los puntos de inicio y fin de la integral completa


*/