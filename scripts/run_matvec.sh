#!/bin/bash
set -e

HOSTFILE="scripts/cluster_hostfile"
CSV="resultados/matvec_resultados.csv"
REPS=3

mkdir -p resultados
echo "algoritmo,N,p,tiempo" > "$CSV"

for N in 1000 2000 4000 8000; do
    echo "=== Multiplicacion matriz-vector N=$N ==="
    for r in 1 2 3; do
        ./bin/matvec_secuencial $N | \
            awk -F, -v N=$N '$1=="SECUENCIAL"{print "secuencial,"N",1,"$3}' >> "$CSV"
    done

    for p in 1 2 4 6; do
        echo "    MPI p=$p"
        for r in 1 2 3; do
            mpirun --hostfile "$HOSTFILE" -np $p ./bin/matvec_mpi $N | \
                awk -F, -v N=$N -v p=$p '$1=="MPI"{print "mpi,"N","p","$4}' >> "$CSV"
        done
    done
done

echo "Resultados guardados en $CSV"
