#!/bin/bash
set -e

mkdir -p bin

mpicc -O2 -o bin/matvec_mpi codigo/mpi/matvec_mpi.c -lm
mpicc -O2 -o bin/oddeven_mpi codigo/mpi/oddeven_mpi.c
gcc -O2 -o bin/matvec_secuencial codigo/secuencial/matvec_secuencial.c -lm
gcc -O2 -o bin/oddeven_secuencial codigo/secuencial/oddeven_secuencial.c

echo "Compilacion completa. Ejecutables en bin/"
