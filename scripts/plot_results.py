import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path

RESULTADOS = Path("resultados")
FIGURAS = Path("informe/figuras")


def procesar(csv_path, nombre):
    df = pd.read_csv(csv_path)
    agrupado = df.groupby(["algoritmo", "N", "p"])["tiempo"].mean().reset_index()

    secuencial = (
        agrupado[agrupado["algoritmo"] == "secuencial"][["N", "tiempo"]]
        .rename(columns={"tiempo": "t1"})
    )
    mpi = (
        agrupado[agrupado["algoritmo"] == "mpi"][["N", "p", "tiempo"]]
        .rename(columns={"tiempo": "tp"})
    )
    merged = mpi.merge(secuencial, on="N")
    merged["speedup"] = merged["t1"] / merged["tp"]
    merged["eficiencia"] = merged["speedup"] / merged["p"]

    Ns = sorted(mpi["N"].unique())
    ps = sorted(mpi["p"].unique())

    # Tiempo vs p
    plt.figure(figsize=(8, 5))
    for N in Ns:
        sub = merged[merged["N"] == N]
        plt.plot(sub["p"], sub["tp"], marker="o", label=f"N={N}")
    plt.xlabel("Número de procesos")
    plt.ylabel("Tiempo (s)")
    plt.title(f"{nombre}: tiempo de ejecución")
    plt.legend()
    plt.grid(True, linestyle="--", alpha=0.7)
    plt.tight_layout()
    plt.savefig(FIGURAS / f"tiempo_{nombre.lower().replace(' ', '_')}.pdf")
    plt.close()

    # Speedup vs p
    plt.figure(figsize=(8, 5))
    for N in Ns:
        sub = merged[merged["N"] == N]
        plt.plot(sub["p"], sub["speedup"], marker="s", label=f"N={N}")
    plt.plot(ps, ps, "k--", label="Speedup ideal")
    plt.xlabel("Número de procesos")
    plt.ylabel("Speedup")
    plt.title(f"{nombre}: speedup")
    plt.legend()
    plt.grid(True, linestyle="--", alpha=0.7)
    plt.tight_layout()
    plt.savefig(FIGURAS / f"speedup_{nombre.lower().replace(' ', '_')}.pdf")
    plt.close()

    # Eficiencia vs p
    plt.figure(figsize=(8, 5))
    for N in Ns:
        sub = merged[merged["N"] == N]
        plt.plot(sub["p"], sub["eficiencia"], marker="^", label=f"N={N}")
    plt.axhline(1, color="k", linestyle="--", label="Eficiencia ideal")
    plt.xlabel("Número de procesos")
    plt.ylabel("Eficiencia")
    plt.title(f"{nombre}: eficiencia")
    plt.legend()
    plt.grid(True, linestyle="--", alpha=0.7)
    plt.tight_layout()
    plt.savefig(FIGURAS / f"eficiencia_{nombre.lower().replace(' ', '_')}.pdf")
    plt.close()

    return merged


if __name__ == "__main__":
    FIGURAS.mkdir(parents=True, exist_ok=True)
    procesar(RESULTADOS / "matvec_resultados.csv", "Matriz-vector")
    procesar(RESULTADOS / "oddeven_resultados.csv", "Odd-Even")
    print("Gráficas generadas en informe/figuras/")
