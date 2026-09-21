# Laboratorio 4: MPI - Multiplicación matriz-vector y Odd-Even Sort

Este repositorio contiene el código fuente, scripts de ejecución e informe en LaTeX para el Laboratorio 4 de Computación Paralela y Distribuida.

## Estructura del proyecto

```
lab4/
├── codigo/
│   ├── common/timer.h          # medición de tiempo portable
│   ├── secuencial/             # versiones secuenciales
│   └── mpi/                    # versiones MPI
├── scripts/
│   ├── compile.sh              # compila todos los programas
│   ├── cluster_hostfile        # hosts y slots del clúster
│   ├── run_matvec.sh           # ejecuta experimentos matriz-vector
│   ├── run_oddeven.sh          # ejecuta experimentos odd-even
│   ├── plot_results.py         # genera gráficas
│   ├── csv_to_latex.py         # genera tablas LaTeX
│   └── setup_python.sh         # instala dependencias de Python
├── resultados/                 # CSVs generados por las ejecuciones
├── informe/                    # archivos LaTeX para Overleaf
└── bin/                        # ejecutables compilados
```

## Requisitos

- Cluster con MPI instalado (`mpicc`, `mpirun`).
- `gcc` para la versión secuencial.
- Acceso SSH sin contraseña entre nodos.
- Python 3 con `pandas` y `matplotlib` (solo en el nodo master).

## Configuración del clúster

Edita `scripts/cluster_hostfile` con los nombres de tus nodos y la cantidad de slots:

```
nodo0 slots=2
nodo1 slots=2
nodo2 slots=2
```

Asegúrate de que todos los nodos puedan ver la carpeta del proyecto a través de NFS.

## Instalación de dependencias de Python

```bash
bash scripts/setup_python.sh
```

## Compilación

```bash
bash scripts/compile.sh
```

## Ejecución de experimentos

```bash
bash scripts/run_matvec.sh
bash scripts/run_oddeven.sh
```

Los resultados se guardan en:

- `resultados/matvec_resultados.csv`
- `resultados/oddeven_resultados.csv`

## Generación de gráficas y tablas LaTeX

```bash
python3 scripts/plot_results.py
python3 scripts/csv_to_latex.py
```

Las gráficas se guardan en `informe/figuras/` y las tablas en `informe/tablas/`.

## Informe en Overleaf

1. Sube **un solo archivo**: `informe/main.tex`.
2. Sube también las figuras PDF que están en `informe/figuras/` (son requeridas por `main.tex`).
3. Compila `main.tex` en Overleaf.

El script `csv_to_latex.py` actualiza las tablas directamente dentro de `main.tex`, por lo que no necesitas subir archivos de tablas adicionales.

## Notas

- Los tamaños de Odd-Even Sort fueron elegidos múltiplos de 6 para garantizar una distribución equitativa entre los 6 cores del clúster.
- Cada experimento se ejecuta 3 veces y se reporta el promedio.
- Si un tamaño de matriz no entra en memoria, reduce `N` en `scripts/run_matvec.sh`.
- Los archivos en `resultados/`, `informe/figuras/` y `informe/tablas/` generados por este repositorio son **datos de ejemplo** para verificar el flujo. Debes reemplazarlos ejecutando los experimentos reales en tu clúster.
