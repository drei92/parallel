#include <stdio.h>
#include <stdlib.h>
#include "../common/timer.h"

void init_array(int* a, int N) {
    for (int i = 0; i < N; i++)
        a[i] = N - i;
}

void compare_exchange(int* a, int i, int j) {
    if (a[i] > a[j]) {
        int tmp = a[i];
        a[i] = a[j];
        a[j] = tmp;
    }
}

int is_sorted(const int* a, int N) {
    for (int i = 1; i < N; i++)
        if (a[i - 1] > a[i])
            return 0;
    return 1;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <N>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);
    int* a = malloc(N * sizeof(int));
    if (!a) {
        fprintf(stderr, "Error: no se pudo reservar memoria\n");
        return 1;
    }

    init_array(a, N);

    double t_start = get_time();
    for (int phase = 0; phase < N; phase++) {
        if (phase % 2 == 0) {
            for (int i = 0; i < N - 1; i += 2)
                compare_exchange(a, i, i + 1);
        } else {
            for (int i = 1; i < N - 1; i += 2)
                compare_exchange(a, i, i + 1);
        }
    }
    double t_end = get_time();

    int ok = is_sorted(a, N);
    printf("SECUENCIAL,%d,%e,%d\n",
           N, t_end - t_start, ok);

    if (N <= 20) {
        for (int i = 0; i < N; i++)
            printf("%d ", a[i]);
        printf("\n");
    }

    free(a);
    return 0;
}
